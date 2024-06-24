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
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

using namespace std;
namespace fs = filesystem;



// this function takes a string and a vector of strings as input and parses
// the string into tokens and stores them in the vector
// some changes were made to accept piplines and redirections but the concept remains the same generally
void Shell::parseCommand(const std::string &input, std::vector<std::string> &args, std::string &inputFile, std::string &outputFile) {
    std::stringstream ss(input);
    std::string token;
    while (ss >> token) {
        if (token == "<") {
            if (ss >> token) {
                inputFile = token;
            }
        } else if (token == ">") {
            if (ss >> token) {
                outputFile = token;
            }
        } else {
            args.push_back(token);
        }
    }
}

void Shell::parsePipeline(const std::string &input, std::vector<std::string> &commands) {
    std::stringstream ss(input);
    std::string command;
    while (getline(ss, command, '|')) {
        commands.push_back(command);
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

void Shell::changeDirectory(const std::string &path)
{
    if (chdir(path.c_str()) == -1) 
    {
        perror("chdir failed");
    }

}


bool Shell::selection(std::string command)
{
            if (command == "") 
                return true;
            if (command == jobsCmd) 
            {
                myJobs();
                return true;
            }
            if (command == historyCmd) 
            {
                myHistory();
                return true;
            }
            if (command == helpCmd) 
            {
                help();
                return true;
            }
            if (command == whoCmd)
            {
                who();
                return true;
            }
    return false;
}

void Shell::printPath()
{
                char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                std::cout << cwd << " " << prompt;
            } else {
                perror("getcwd failed");
            }
}

void Shell::executeCommandsWithPipes(std::vector<std::string> &commands, bool isBackground) {
    int numCommands = commands.size();
    int pipefds[2 * (numCommands - 1)];

    for (int i = 0; i < numCommands - 1; i++) {
        if (pipe(pipefds + i*2) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    int j = 0;
    for (int i = 0; i < numCommands; i++) {
        std::vector<std::string> args;
        std::string inputFile, outputFile;
        parseCommand(commands[i], args, inputFile, outputFile);

        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) {
                if (dup2(pipefds[j-2], 0) < 0) {  // Redirect stdin to the previous pipe's read end
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            if (i < numCommands - 1) {
                if (dup2(pipefds[j+1], 1) < 0) {  // Redirect stdout to the next pipe's write end
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            for (int k = 0; k < 2 * (numCommands - 1); k++) {
                close(pipefds[k]);
            }

            handleRedirection(inputFile, outputFile);

            std::vector<char*> c_args;
            for (const auto& arg : args) {
                c_args.push_back(const_cast<char*>(arg.c_str()));
            }
            c_args.push_back(nullptr);

            std::string fullPath;
            if (!Utils::findExe(c_args[0], fullPath)) {
                std::cerr << "Command not found: " << c_args[0] << std::endl;
                exit(EXIT_FAILURE);
            }

            execv(fullPath.c_str(), c_args.data());
            perror("execv");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        
        if (i == numCommands - 1 && isBackground) {
            addJob(pid, commands[i]);
        }

        j += 2;
    }

    for (int i = 0; i < 2 * (numCommands - 1); i++) {
        close(pipefds[i]);
    }

    if (!isBackground) {
        for (int i = 0; i < numCommands; i++) {
            wait(NULL);
        }
    }
}

// this function takes two strings as input and redirects the input and output of the shell to the files
void Shell::handleRedirection(const std::string &inputFile, const std::string &outputFile)
{
    if (!inputFile.empty()) 
    {
        int fd = open(inputFile.c_str(), O_RDONLY); 
        if (fd == -1) 
        {
            perror("open input file failed");
            exit(-1);
        }
        dup2(fd, STDIN_FILENO);  // Redirect STDIN
        close(fd);
    }

    if (!outputFile.empty()) 
    {
        int fd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644); //readonly , truncates the file to 0 , 0644 is the permission for the file
        if (fd == -1) 
        {
            perror("open output file failed");
            exit(-1);
        }
        dup2(fd, STDOUT_FILENO);  // Redirect STDOUT
        close(fd);
    }
}

// my main method for this shell program
// it goes over the input string and processes it accordingly
// it checks for in program commands and also path commands
void Shell::inputLoop() {
    bool running = true;
    while (running) {
        try {
            printPath();

            char* input = readline("");
            if (!input) break;

            std::string command(input);
            add_history(command.c_str());
            Utils::addCommandToHistory(command);

            if (command == exitCmd) {
                running = false;
                break;
            }
            if (selection(command)) {
                continue;
            }

            command = Utils::parseEnvironmentVariables(command);

            bool isRunningInBackground = false;
            if (!command.empty() && command.back() == '&') {
                isRunningInBackground = true;
                command.pop_back();
                if (!command.empty() && command.back() == ' ') {
                    command.pop_back();
                }
            }

            std::vector<std::string> args;
            std::string inputFile, outputFile;
            std::vector<std::string> commands;
            parsePipeline(command, commands);

            if (commands.size() > 1) {
                executeCommandsWithPipes(commands, isRunningInBackground);
                continue;
            }

            parseCommand(command, args, inputFile, outputFile);

            if (args.empty()) continue;

            if (args[0] == cdCmd) {
                if (args.size() < 2) {
                    std::cerr << "cd: missing argument" << std::endl;
                } else {
                    changeDirectory(args[1]);
                }
                continue;
            }

            std::string fullPath;
            if (!Utils::findExe(args[0], fullPath)) {
                std::cerr << "Command not found: " << args[0] << std::endl;
                continue;
            }

            std::vector<char*> c_args;
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
                    handleRedirection(inputFile, outputFile);
                    execv(fullPath.c_str(), c_args.data());
                    perror("execv failed");
                    exit(-1);
                default:
                    if (isRunningInBackground) {
                        addJob(pID, command);
                        std::cout << "Running process in background, PID: " << pID << std::endl;
                    } else {
                        waitpid(pID, nullptr, 0);
                    }
                    break;
            }

        } catch (const std::exception& e) {
            std::cerr << "Command not found" << std::endl;
        }
    }
}

void Shell::run() 
{


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