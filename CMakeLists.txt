cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
set(PROJECT_NAME LodProject)
# Set project
project(
    ${PROJECT_NAME}
    VERSION 1.0.0
)
# C++ Version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Add GLM support
include_directories(../external/)
# Set output paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# MPI projects
add_subdirectory(./LodGenerator/ LodGenerator)
add_subdirectory(./LodTerminal/ LodTerminal)