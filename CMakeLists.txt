cmake_minimum_required(VERSION 3.10)

# Set the project name
project(ExcellenteamShell)

# Specify the source files
set(SOURCES src/main.cpp src/Shell.cpp src/Command.cpp)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/header_files)

