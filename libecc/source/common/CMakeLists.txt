# 
# Name the project
PROJECT(LIBC)

cmake_minimum_required(VERSION 2.6)

SET(LIBECC_DIR ${PROJECT_SOURCE_DIR})
# Tell CMake to process the sub-directories
add_subdirectory(libc)
add_subdirectory(libm)
add_subdirectory(soft-fp)
add_subdirectory(soft-int)
