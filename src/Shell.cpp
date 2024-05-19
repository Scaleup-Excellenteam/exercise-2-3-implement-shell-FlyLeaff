#include "Shell.h"
#include "Utils.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <thread>
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;
namespace fs = filesystem;




void Shell::parseCommand(const string &input, vector<string> &args) {
    stringstream ss(input);
    string token;
    while (ss >> token) {
        args.push_back(token);
    }
}

void Shell::addJob(pid_t pid, const string& cmd) {
    Job job = {pid, cmd, "Running"};
    bgJobs[pid] = job;
}

void Shell::updateJobStatus() {
    for (auto it = bgJobs.begin(); it != bgJobs.end();) {
        int status;
        pid_t result = waitpid(it->first, &status, WNOHANG);
        if (result == 0) {
            it->second.status = "Running";
            ++it;
        } else if (result == -1) {
            perror("waitpid failed");
            it = bgJobs.erase(it);
        } else {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                it->second.status = "Stopped";
            }
            ++it;
        }
    }
}

void Shell::myJobs() {
    updateJobStatus();
    for (const auto& job : bgJobs) {
        cout << "PID: " << job.second.pid << ", Command: " << job.second.command << ", Status: " << job.second.status << endl;
    }
}

void Shell::inputLoop() {
    while (true) {
        char* input = readline("shell> ");
        if (input == nullptr) break;
        
        string command(input);
        add_history(command.c_str());
        Utils::addCommandToHistory(command);
      
        if (command == "") continue;
        if (command == "exit") break;
        if (command == "myjobs") 
        {
            myJobs();
            continue;
        }
        if (command == "myhistory") 
        {
            myHistory();
            continue;
        }

        command = Utils::parseEnvironmentVariables(command);


        vector<string> args;
        parseCommand(command, args);
        bool runInBackground = false;
        if (!args.empty() && args.back().back() == '&') {
            runInBackground = true;
            if (args.back().length() == 1) {
                args.pop_back();
            } else {
                args.back().pop_back();
            }
        }

        string fullPath;
        if (!Utils::findExe(args[0], fullPath)) {
            cerr << "Command not found: " << args[0] << endl;
            continue;
        }

        vector<char*> c_args;
        for (const auto& arg : args) {
            c_args.push_back(const_cast<char*>(arg.c_str()));
        }
        c_args.push_back(nullptr);

        pid_t pID = fork();
        switch (pID) {
            case -1:
                perror("fork failed");
                break;
            case 0:
                setpgrp();
                execv(fullPath.c_str(), c_args.data());
                perror("execv failed");
                exit(-1);
            default:
                if (runInBackground) {
                    addJob(pID, command);
                    cout << "Running process in background, PID: " << pID << endl;
                } else {
                    waitpid(pID, nullptr, 0);
                }
                break;
        }
    }
}

void Shell::run() {
    system("reset");
        rl_clear_history();

    Utils::resetHistoryFile();
    thread inputThread(&Shell::inputLoop, this);
    inputThread.join();



}


void Shell::myHistory() {
    Utils::showHistory();
}