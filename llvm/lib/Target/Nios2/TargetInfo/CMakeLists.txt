include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMNios2Info
  Nios2TargetInfo.cpp
  )

add_dependencies(LLVMNios2Info Nios2CodeGenTable_gen)
