cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
set(PROJECT_NAME LodGenerator)
# Set project
project(
    ${PROJECT_NAME}
    VERSION 1.0.0
)
# Set include directories
include_directories(./inc)
# Add GLM support
include_directories(../external/)
# Set Source Files
set(SOURCE_FL
    ./src/algorithms/basic_alg.cpp
    ./src/lod_math.cpp
    ./src/core.cpp
    ./src/mesh.cpp
)
# Add target library
add_library(${PROJECT_NAME} ${SOURCE_FL})