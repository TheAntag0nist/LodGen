cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
set(PROJECT_NAME LodGenerator)
# Set project
project(
    ${PROJECT_NAME}
    VERSION 1.0.0
)
# Set include directories
include_directories(./inc)
# Set Source Files
set(SOURCE_FL
    ./src/core.cpp
)
# Add target library
add_library(${PROJECT_NAME} ${SOURCE_FL})