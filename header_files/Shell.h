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
    void changeDirectory(const std::string &path);


    const std::string prompt = "myshell> ";
    const std::string exitCmd = "exit";
    const std::string jobsCmd = "myjobs";
    const std::string historyCmd = "myhistory";
    const std::string helpCmd = "help";
    const std::string whoCmd = "who";
    const std::string cdCmd = "cd";  
    
};

#endif // SHELL_H
