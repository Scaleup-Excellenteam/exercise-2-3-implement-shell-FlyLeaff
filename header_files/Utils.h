#ifndef UTILS_H
#define UTILS_H

#include <string>

class Utils 
{
public:
    static const std::string historyPath;

    // Method to find the executable path for a given command
    static bool findExe(const std::string &command, std::string &fullPath);

    // Method to reset the history file
    static void resetHistoryFile();

    // Method to add a command to the history file
    static void addCommandToHistory(const std::string &command);

    // Method to display the history from the history file
    static void showHistory();

    // Method to parse environment variables within a given command string
    static std::string parseEnvironmentVariables(const std::string &command);
};

#endif // UTILS_H
