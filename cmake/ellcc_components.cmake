# Definitions for building ELLCC components.

# Include paths.
include_directories(${ELLCC_SOURCE_DIR}/ellcc/Basic
                    ${ELLCC_SOURCE_DIR}/ellcc/SMBase)

# Get LLVM definitions.
execute_process(COMMAND llvm-config --cxxflags OUTPUT_VARIABLE LLVM_CXXFLAGS)
add_definitions(${LLVM_CXXFLAGS})
