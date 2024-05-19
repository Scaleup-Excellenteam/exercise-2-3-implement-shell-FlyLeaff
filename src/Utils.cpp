#include "Utils.h"
#include <iostream>
#include <sstream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

const string Utils::historyPath = "/tmp/myshell_history.txt";
bool Utils::findExe(const string &command, string &fullPath) 
{
    // this section gets the path enviroment variables value to search if the command is in there
    const char *pathEnv = getenv("PATH");
    if (!pathEnv) {
        cerr << "PATH environment variable not found" << endl;
        return false;
    }

    // here is the actual search for the executable i wanna run
    stringstream ss(pathEnv);
    string path;
    while (getline(ss, path, ':')) {
        fs::path p = path;
        p /= command;
        if (fs::exists(p) && fs::is_regular_file(p) && ((fs::status(p).permissions() & fs::perms::owner_exec) != fs::perms::none)) {
            fullPath = p.string();
            return true; // exe found
        }
    }
    // only get here if i didnt find the exe in the search
    return false;
}

#include <fstream>

void Utils::resetHistoryFile() 
{
    ofstream historyFile(historyPath);
    if (!historyFile) {
        cerr << "Failed to open history file" << endl;
        return;
    }
    historyFile.close();
}

void Utils::addCommandToHistory(const string &command) 
{
    ofstream historyFile(historyPath, ios::app);
    if (!historyFile) {
        cerr << "Failed to open history file" << endl;
        return;
    }
    historyFile << command << endl;
    historyFile.close();
}


void Utils::showHistory() 
{
    ifstream historyFile(historyPath);
    if (!historyFile) {
        cerr << "Failed to open history file" << endl;
        return;
    }
    string line;
    int i = 1;
    while (getline(historyFile, line)) {
        cout << i++ << ". " << line << endl;
    }
    historyFile.close();
}


// long ass function with some help from gpt :D
// it goes over the whole input string and if theres a $ or ${} in it
// the function translates that into whats in the actual variable.
string Utils::parseEnvironmentVariables(const std::string &command) 
{
    std::string result = command;
    size_t startPos = result.find('$');
    
    while (startPos != std::string::npos) 
    {
        size_t endPos = startPos + 1;
        
        // Handle variables enclosed in ${}
        if (endPos < result.size() && result[endPos] == '{') 
        {
            endPos = result.find('}', startPos + 2);
            if (endPos != std::string::npos) 
            {
                std::string varName = result.substr(startPos + 2, endPos - startPos - 2);
                const char* envValue = getenv(varName.c_str());
                if (envValue) 
                    result.replace(startPos, endPos - startPos + 1, envValue);

                else result.replace(startPos, endPos - startPos + 1, "");
                startPos = result.find('$', startPos);
            } 
            else break;
        } 
        else 
        {
            while (endPos < result.size() && (isalnum(result[endPos]) || result[endPos] == '_')) 
                endPos++;
            std::string varName = result.substr(startPos + 1, endPos - startPos - 1);
            const char* envValue = getenv(varName.c_str());
            if (envValue) result.replace(startPos, endPos - startPos, envValue);
            else result.replace(startPos, endPos - startPos, "");
            startPos = result.find('$', startPos);
        }
    }
    return result;
}