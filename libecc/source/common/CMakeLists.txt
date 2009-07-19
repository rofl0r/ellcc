# 
# Name the project
PROJECT(LIBECC)

cmake_minimum_required(VERSION 2.6)

# Tell CMake to process the sub-directories
add_subdirectory(libc)
add_subdirectory(libm)
add_subdirectory(soft-fp)
add_subdirectory(soft-int)
