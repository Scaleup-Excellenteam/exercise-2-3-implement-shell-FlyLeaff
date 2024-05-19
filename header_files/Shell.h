#ifndef SHELL_H
#define SHELL_H

#include <map>
#include <vector>
#include <string>
#include "Command.h"

class Shell {
public:
    void run();
    void parseCommand(const string &input, vector<string> &args);
    void myJobs();
    void addJob(pid_t,const string& command);

private:
    struct Job
    {
        pid_t pid;
        string command,status;
    };
    map<pid_t,Job> bgJobs;


    void updateJobStatus();
};

#endif // SHELL_H
