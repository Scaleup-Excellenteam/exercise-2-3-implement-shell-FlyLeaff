#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <vector>
#include <map>
#include <sys/types.h>

class Shell 
{
public:


    void run();


private:
    struct Job 
    {
        pid_t pid;
        std::string command;
        std::string status;
    };


    std::map<pid_t, Job> bgJobs;
    
    void parseCommand(const std::string &input, std::vector<std::string> &args);
    void addJob(pid_t pid, const std::string &cmd);
    void updateJobStatus();
    void myJobs();
    void inputLoop();
    void myHistory();
    void help();
    void who();
    
};

#endif // SHELL_H
