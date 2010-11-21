set(LLVM_TARGET_DEFINITIONS Nios2.td)

tablegen(Nios2GenRegisterInfo.h.inc -gen-register-desc-header)
tablegen(Nios2GenRegisterNames.inc -gen-register-enums)
tablegen(Nios2GenRegisterInfo.inc -gen-register-desc)
tablegen(Nios2GenInstrNames.inc -gen-instr-enums)
tablegen(Nios2GenInstrInfo.inc -gen-instr-desc)
tablegen(Nios2GenAsmWriter.inc -gen-asm-writer)
tablegen(Nios2GenDAGISel.inc -gen-dag-isel)
tablegen(Nios2GenCallingConv.inc -gen-callingconv)
tablegen(Nios2GenSubtarget.inc -gen-subtarget)

add_llvm_target(Nios2CodeGen
  Nios2InstrInfo.cpp
  Nios2ISelDAGToDAG.cpp
  Nios2ISelLowering.cpp
  Nios2RegisterInfo.cpp
  Nios2Subtarget.cpp
  Nios2MCAsmInfo.cpp
  Nios2TargetMachine.cpp
  Nios2TargetObjectFile.cpp
  Nios2SelectionDAGInfo.cpp
  )

target_link_libraries (LLVMNios2CodeGen LLVMSelectionDAG)
