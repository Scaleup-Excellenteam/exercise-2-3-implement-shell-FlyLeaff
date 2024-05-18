#ifndef SHELL_H
#define SHELL_H

#include <vector>
#include <string>
#include "Command.h"

class Shell {
public:
    void run();
    void parseCommand(const string &input, vector<string> &args);
};

#endif // SHELL_H
