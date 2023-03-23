cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
set(PROJECT_NAME LodTerminal)
# Set project
project(
    ${PROJECT_NAME}
    VERSION 1.0.0
)
# Set include directories
include_directories(./inc)
include_directories(../LodGenerator/inc)
# Set Source Files
set(SOURCE_FL
    ./src/main.cpp
	./src/terminal.cpp
)
# Add target library
add_executable(${PROJECT_NAME} ${SOURCE_FL})
target_link_libraries(${PROJECT_NAME} LodGenerator)