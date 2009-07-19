# 
# Name the project
PROJECT(LIBC)

cmake_minimum_required(VERSION 2.6)

# Tell CMake to process the sub-directories
add_subdirectory(argz)
add_subdirectory(ctype)
add_subdirectory(errno)
add_subdirectory(iconv)
add_subdirectory(locale)
add_subdirectory(machine)
add_subdirectory(misc)
add_subdirectory(posix)
add_subdirectory(reent)
add_subdirectory(search)
add_subdirectory(signal)
add_subdirectory(stdio)
add_subdirectory(stdlib)
add_subdirectory(string)
add_subdirectory(sys)
add_subdirectory(syscalls)
