cmake_minimum_required(VERSION 3.10)

# Set the project name
project(ExcellenteamShell)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(ExcellenteamShell src/main.cpp "src/header files/Shell.h" "src/Command.cpp" "src/header files/Command.h")

# Include directories
target_include_directories(ExcellenteamShell PRIVATE include)
