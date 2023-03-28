cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
set(PROJECT_NAME LodGenerator)
# Set project
project(
    ${PROJECT_NAME}
    VERSION 1.0.0
)
# Find OpenCL
find_package(OpenCL REQUIRED)
INCLUDE_DIRECTORIES(${OpenCL_INCLUDE_DIRS})
# Set include directories
include_directories(./inc)
# Add GLM support
include_directories(../external/)
# Set Source Files
set(SOURCE_FL
    ./src/algorithms/vertex_cluster.cpp
    ./src/algorithms/basic_alg.cpp
    ./src/gpu_math/gpu_test.cpp
    ./src/gpu_math/gpu_core.cpp
    ./src/math/lod_math.cpp
    ./src/core_config.cpp
    ./src/core.cpp
    ./src/mesh.cpp
)
# Add target library
add_library(${PROJECT_NAME} ${SOURCE_FL})
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)