//===-- Nios2TargetMachine.cpp - Define TargetMachine for Nios2 -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about Nios2 target spec.
//
//===----------------------------------------------------------------------===//

#include "Nios2.h"
#include "Nios2MCAsmInfo.h"
#include "Nios2TargetMachine.h"
#include "llvm/PassManager.h"
#include "llvm/Target/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeNios2Target() {
  // Register the target.
  RegisterTargetMachine<Nios2TargetMachine> X(TheNios2Target);
  RegisterTargetMachine<Nios2ebTargetMachine> Y(TheNios2ebTarget);
  RegisterAsmInfo<Nios2MCAsmInfo> A(TheNios2Target);
  RegisterAsmInfo<Nios2MCAsmInfo> B(TheNios2ebTarget);
}

// DataLayout --> Big-endian, 32-bit pointer/ABI/alignment
// The stack is always 4 byte aligned
// On function prologue, the stack is created by decrementing
// its pointer. Once decremented, all references are done with positive
// offset from the stack/frame pointer, using StackGrowsUp enables 
// an easier handling.
// Using CodeModel::Large enables different CALL behavior.
Nios2TargetMachine::
Nios2TargetMachine(const Target& T, const std::string &TT, const std::string &FS,
                   bool isLittle = true):
  LLVMTargetMachine(T, TT),
  Subtarget(TT, FS, isLittle), 
  DataLayout(isLittle ? std::string("e-p:32:32:32-i8:8:32-i16:16:32-n32") :
                        std::string("E-p:32:32:32-i8:8:32-i16:16:32-n32")), 
  InstrInfo(*this), 
  FrameInfo(TargetFrameInfo::StackGrowsUp, 4, 0),
  TLInfo(*this), TSInfo(*this) {
}

Nios2ebTargetMachine::
Nios2ebTargetMachine(const Target& T, const std::string &TT,
                     const std::string &FS) :
  Nios2TargetMachine(T, TT, FS, true) {}

// Install an instruction selector pass using 
// the ISelDag to gen Nios2 code.
bool Nios2TargetMachine::
addInstSelector(PassManagerBase &PM, CodeGenOpt::Level OptLevel) 
{
  PM.add(createNios2ISelDag(*this));
  return false;
}

// Implemented by targets that want to run passes immediately before 
// machine code is emitted. return true if -print-machineinstrs should 
// print out the code after the passes.
bool Nios2TargetMachine::
addPreEmitPass(PassManagerBase &PM, CodeGenOpt::Level OptLevel) 
{
  return true;
}
