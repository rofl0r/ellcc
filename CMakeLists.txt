#
# 
# Name the project
PROJECT(ELLCC)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug)
include_directories(${ELLCC_SOURCE_DIR}/smbase)
execute_process(COMMAND llvm-config --cxxflags OUTPUT_VARIABLE LLVM_CXXFLAGS)
add_definitions(${LLVM_CXXFLAGS})

# Tell CMake to process the sub-directories
add_subdirectory(basic)
add_subdirectory(smbase)
