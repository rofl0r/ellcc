//===-- ARMTargetInfo.cpp - ARM Target Implementation ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ARM.h"
#include "llvm/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheARMTarget, llvm::TheARMEBTarget, llvm::TheThumbTarget;

extern "C" void LLVMInitializeARMTargetInfo() { 
  RegisterTarget<Triple::arm, /*HasJIT=*/true>
    X(TheARMTarget, "arm", "ARM");

  RegisterTarget<Triple::armeb, /*HasJIT=*/true>
    Y(TheARMEBTarget, "armeb", "ARMEB");

  RegisterTarget<Triple::thumb, /*HasJIT=*/true>
    Z(TheThumbTarget, "thumb", "Thumb");
}
