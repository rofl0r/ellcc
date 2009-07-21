# Definitions for building ELLCC components.

# Include paths.
include_directories(${ELLCC_SOURCE_DIR}/basic
                    ${ELLCC_SOURCE_DIR}/smbase)

# Get LLVM definitions.
execute_process(COMMAND llvm-config --cxxflags OUTPUT_VARIABLE LLVM_CXXFLAGS)
add_definitions(${LLVM_CXXFLAGS})
