#include "Shell.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sstream>
#include <cstdlib>
#include <filesystem>

using namespace std;
namespace fs = filesystem;


// This method translates the input string 
// into the string vector for the execv that will be called in run.
// its used to split the command and args of each input line
void Shell::parseCommand(const string &input, vector<string> &args) {
    stringstream ss(input);
    string token;
    while (ss >> token) {
        args.push_back(token);
    }
}

void Shell::addJob(pid_t pid, const string& cmd)
{

    Job job = {pid,cmd, "Running"};
    bgJobs[pid] = job;
}

void Shell::updateJobStatus()
{
    
}
void Shell::myJobs() {
    updateJobStatus();
    for (const auto& job : bgJobs) {
        cout << "PID: " << job.second.pid << ", Command: " << job.second.command << ", Status: " << job.second.status << endl;
    }
}

// the main method for this shell essentially
// it takes care of looping till exit is entered, taking inputs from user and delegating 
// tasks to the sub-processes 
void Shell::run() 
{
    string command;
    while (true) 
    {
        cout << "Enter command: ";
        getline(cin, command);

        if (command == "exit") break;
        if (command == "myjobs")
        {
            myJobs();
            continue;
        }

        
        vector<string> args;
        parseCommand(command, args);
        bool runInBackground = false;
        if(!args.empty() && args.back() == "&")
        {
            runInBackground = true;
            args.pop_back(); // to get rid of the '&'
        }

        string fullPath;
        if (!Command::findExe(args[0], fullPath)) 
        {
            cerr << "Command not found: " << args[0] << endl;
            continue;
        }

        // Convert vector<string> to array of char* for execv
        vector<char*> c_args;
        for (size_t i = 0; i < args.size(); ++i) 
        {
            c_args.push_back(const_cast<char*>(args[i].c_str()));
        }
        c_args.push_back(nullptr);

        pid_t pID = fork();

        switch (pID) 
        {
            case -1:
                perror("fork failed");
                break;
            case 0:
                // Child process
                setpgrp();
                // Use execv to execute the command
                execv(fullPath.c_str(), c_args.data());
                // If execv fails
                perror("execv failed");
                exit(-1);
            default:
                // Parent process
                wait(nullptr);
                break;
        }
    }
}
