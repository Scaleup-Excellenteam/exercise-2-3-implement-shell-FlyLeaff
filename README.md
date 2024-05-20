# Build A Shell

A simple shell built in C++ within WSL for the ExcellenTeam linux course

## Table of Contents
- [Technical Capabilities](#technical-capabilities)
- [Commands](#commands)
  - [myjobs](#myjobs)
  - [myhistory](#myhistory)
- [Building the Project](#building-the-project)


## Technical Capabilities

This project implements a basic shell in C++ with the following capabilities:

- **Command Execution:** The shell reads commands from the user and executes them using `fork` and `exec` system calls.
- **Path Resolution:** It searches for executable files in the directories listed in the `PATH` environment variable.
- **Background Execution:** Supports running processes in the background using the `&` operator.
- **Environment Variables:** Parses and expands environment variables within commands.
- **Command History:** Maintains a history of executed commands which can be viewed using the `myhistory` command.
                            _Command history will be saved on temp_

## Commands

### myjobs

This command allows the user to see all the processes running in the background. It displays the command, status, and `pid` of each background process.

### myhistory

This command displays the history of all executed commands, showing the full list of commands that have been entered into the shell.

### &

Ending any command with '&' will make it run in the background via another process

### exit

Use "exit" to exit the shell

### help 

use "help" to get a function list

### who

gives me some credit and prints a link to the github page

---

For detailed information on how to build and run the project, please refer to the sections below.

### Building the Project

To build the project, follow these steps:

1. **Install Dependencies:**
   Make sure you have the necessary libraries installed.
   Two files are provided:
   requirements.txt for the dependency list
   and
   Install_Requirements.sh - a small script to auto install whats needed
2. **Configure CMake:**
   The CMakeLists file was provided
3. **Build**
   run the following commands:


```
  #### Step 1: install dependencies
  chmod +x Install_Requirements.sh
  ./Install_Requirements.sh
  
  #### Step 2: Clone Repository
  git clone https://github.com/yourusername/Build-A-Shell.git
  cd Build-A-Shell
  
  #### Step 4: Build the Project
  mkdir build
  cd build
  cmake ..
  make

  #### Step 5: Run the Application 
  ./ExcellenteamShell
```
---

Readme made with help from chatgpt

chatgpt helped sections are mentioned in comments within the project
