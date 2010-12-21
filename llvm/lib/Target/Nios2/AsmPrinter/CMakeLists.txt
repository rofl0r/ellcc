include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  )

add_llvm_library(LLVMNios2AsmPrinter
  Nios2AsmPrinter.cpp
  )

add_dependencies(LLVMNios2AsmPrinter Nios2CodeGenTable_gen)
