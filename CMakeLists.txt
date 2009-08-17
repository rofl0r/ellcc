# 
# Build the ELLCC compiler collection.
PROJECT(ELLCC)
cmake_minimum_required(VERSION 2.6)

# Define the build type.
set(CMAKE_BUILD_TYPE Debug)

# Define the library type
set(ELLCC_LIBRARY_TYPE ) # SHARED)
# Define the runtime library source directory.
set(COMPILER_RT_DIR  ${ELLCC_SOURCE_DIR}/../compiler-rt)
# Define the target processors.
file(READ ${ELLCC_SOURCE_DIR}/targets ELLCC_TARGETS)
# Turn the targets into a cmake list.
string(REGEX REPLACE [^a-zA-Z0-9_-]+ ";" ELLCC_TARGETS ${ELLCC_TARGETS})
set(LINUX_TARGETS arm-elf mips-elf powerpc-elf powerpc64-elf sparc-elf x86-elf x86_64-elf)

# Tell CMake to process the sub-directories
add_subdirectory(basic)
add_subdirectory(smbase)

# Now we can build the runtime libraries.
add_subdirectory(libecc)

# Now (finally!) we can test programs.
add_subdirectory(test EXCLUDE_FROM_ALL)
