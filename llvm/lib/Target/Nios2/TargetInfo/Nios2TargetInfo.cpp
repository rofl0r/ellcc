//===-- Nios2TargetInfo.cpp - Nios2 Target Implementation -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Nios2.h"
#include "llvm/Module.h"
#include "llvm/Target/TargetRegistry.h"
using namespace llvm;

Target llvm::TheNios2Target, llvm::TheNios2ebTarget;

extern "C" void LLVMInitializeNios2TargetInfo() { 
  RegisterTarget<Triple::nios2> X(TheNios2Target, "nios2", "Nios2");
  RegisterTarget<Triple::nios2eb> Y(TheNios2ebTarget, "nios2eb", "Nios2eb");

}
