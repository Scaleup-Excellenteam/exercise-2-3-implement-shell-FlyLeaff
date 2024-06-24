#ifndef UTILS_H
#define UTILS_H

#include <string>

class Utils 
{
public:
    static const std::string historyPath;

    static bool findExe(const std::string &command, std::string &fullPath);

    static void resetHistoryFile();

    static void addCommandToHistory(const std::string &command);

    static void showHistory();

    static std::string parseEnvironmentVariables(const std::string &command);
};

#endif // UTILS_H
