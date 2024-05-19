cmake_minimum_required(VERSION 3.10)

# Set the project name
project(ExcellenteamShell)

# Specify the source files
set(SOURCES src/main.cpp src/Shell.cpp src/Utils.cpp)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/header_files)

# Manually specify the include directory for readline
include_directories(/usr/include)

# Manually link the readline library
target_link_libraries(${PROJECT_NAME} readline)
