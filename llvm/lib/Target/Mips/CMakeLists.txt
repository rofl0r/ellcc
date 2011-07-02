set(LLVM_TARGET_DEFINITIONS Mips.td)

tablegen(MipsGenRegisterInfo.inc -gen-register-info)
tablegen(MipsGenInstrInfo.inc -gen-instr-info)
tablegen(MipsGenAsmWriter.inc -gen-asm-writer)
tablegen(MipsGenDAGISel.inc -gen-dag-isel)
tablegen(MipsGenCallingConv.inc -gen-callingconv)
tablegen(MipsGenSubtargetInfo.inc -gen-subtarget)
tablegen(MipsGenIntrinsics.inc -gen-tgt-intrinsic)

add_llvm_target(MipsCodeGen
  MipsAsmPrinter.cpp
  MipsDelaySlotFiller.cpp
  MipsEmitGPRestore.cpp
  MipsExpandPseudo.cpp
  MipsInstrInfo.cpp
  MipsISelDAGToDAG.cpp
  MipsISelLowering.cpp
  MipsFrameLowering.cpp
  MipsMCAsmInfo.cpp
  MipsRegisterInfo.cpp
  MipsSubtarget.cpp
  MipsTargetMachine.cpp
  MipsTargetObjectFile.cpp
  MipsSelectionDAGInfo.cpp
  MipsIntrinsicInfo.cpp
  )

add_subdirectory(TargetInfo)
