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



// this function takes a string and a vector of strings as input and parses
// the string into tokens and stores them in the vector
void Shell::parseCommand(const string &input, vector<string> &args) 
{
    stringstream ss(input);
    string token;
    while (ss >> token) {
        args.push_back(token);
    }
}

void Shell::addJob(pid_t pid, const string& cmd) 
{
    Job job = {pid, cmd, "Running"};
    bgJobs[pid] = job;
}

// this function goes over the bgJobs map and updates the status of each job
void Shell::updateJobStatus() 
{
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
            if (WIFEXITED(status) || WIFSIGNALED(status)) // this signal is sent to a process when its controlling terminal is closed
                it = bgJobs.erase(it); // no use in a stopped job in a currently running job list
            else 
                ++it;
            
        }
    }
}

// this simply goes over the pid - string map and prints out the pid, command and status of each job
void Shell::myJobs() 
{
    updateJobStatus();
    for (const auto& job : bgJobs) {
        cout << "PID: " << job.second.pid << ", Command: " << job.second.command << ", Status: " << job.second.status << endl;
    }
}


// my main method for this shell program
// it goes over the input string and processes it accordingly
// it checks for in program commands and also path commands
void Shell::inputLoop() 
{
    while (true) {
        char* input = readline("");
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
        if (command == "help") 
        {
            help();
            continue;
        }
        if (command == "who") 
        {
            who();
            continue;
        }

        command = Utils::parseEnvironmentVariables(command);

        //process command
        vector<string> args;
        parseCommand(command, args);
        bool runInBackground = false;
        if (!args.empty() && args.back().back() == '&') 
        {
            runInBackground = true;
            if (args.back().length() == 1) {
                args.pop_back();
            } else {
                args.back().pop_back();
            }
        }


        //check in path for command
        string fullPath;
        if (!Utils::findExe(args[0], fullPath)) 
        {
            cerr << "Command not found: " << args[0] << endl;
            continue;
        }

        vector<char*> c_args;
        for (const auto& arg : args) 
            c_args.push_back(const_cast<char*>(arg.c_str()));
        
        c_args.push_back(nullptr);


        //fork to child process and execute via execv
        pid_t pID = fork();
        switch (pID) 
        {
            case -1:
                perror("fork failed");
                break;
            case 0:
                setpgrp();
                execv(fullPath.c_str(), c_args.data());
                perror("execv failed");
                exit(-1);
            default:
                if (runInBackground) 
                {
                    addJob(pID, command);
                    cout << "Running process in background, PID: " << pID << endl;
                } 
                else 
                    waitpid(pID, nullptr, 0);
                
                break;
        }
    }
}

void Shell::run() 
{
    // system("reset"); // this is in place due to an issue with my vscode terminal and can be removed
    // //TODO remove this shit


    Utils::resetHistoryFile();
    thread inputThread(&Shell::inputLoop, this);
    inputThread.join();



}


void Shell::myHistory() 
{
    Utils::showHistory();
}

void Shell::help()
{
    cout << "Function list: \nhelp - displays this.\n'&' - end args in a '&' sign to run in background."
    << endl << "myjobs - displays a list of currently running tasks in background."
    << endl << "myhistory - display commands used since shell start.\n"
    << endl << "who - displays credits and git link.";
}

void Shell::who()
{
    cout << "Made by Kohav Buskila - browse the git for more info: https://github.com/Scaleup-Excellenteam/exercise-2-3-implement-shell-FlyLeaff/tree/main";
}