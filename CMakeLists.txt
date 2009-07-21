# 
# Build the ELLCC compiler collection.
PROJECT(ELLCC)
cmake_minimum_required(VERSION 2.6)

# Define the build type.
set(CMAKE_BUILD_TYPE Debug)

# Define the library type
set(ELLCC_LIBRARY_TYPE ) # SHARED)

# Define the target processors.
file(READ ${ELLCC_SOURCE_DIR}/targets ELLCC_TARGETS)
# Turn the targets into a cmake list.
string(REGEX REPLACE [^a-zA-Z0-9_-]+ ";" ELLCC_TARGETS ${ELLCC_TARGETS})

include_directories(${ELLCC_SOURCE_DIR}/smbase)

# Get LLVM definitions.
execute_process(COMMAND llvm-config --cxxflags OUTPUT_VARIABLE LLVM_CXXFLAGS)
add_definitions(${LLVM_CXXFLAGS})

# Tell CMake to process the sub-directories
add_subdirectory(basic)
add_subdirectory(smbase)

# Now we can build the runtime libraries.
add_subdirectory(libecc)
