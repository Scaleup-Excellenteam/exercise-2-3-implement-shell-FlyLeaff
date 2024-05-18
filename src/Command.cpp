#include "Command.h"
#include <iostream>
#include <sstream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

bool Command::findExe(const string &command, string &fullPath) 
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
