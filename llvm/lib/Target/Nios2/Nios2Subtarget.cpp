//===- Nios2Subtarget.cpp - Nios2 Subtarget Information -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Nios2 specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

#include "Nios2Subtarget.h"
#include "Nios2.h"
#include "Nios2GenSubtarget.inc"
using namespace llvm;

Nios2Subtarget::Nios2Subtarget(const std::string &TT, const std::string &FS, 
                               bool little) : 
  IsLittle(little),
  HasMul(true), HasMulx(false), HasDiv(false),
  IsLinux(true)
{
  std::string CPU = "nios2";

  // Parse features string.
  ParseSubtargetFeatures(FS, CPU);
  // Is the target system Linux ?
  if (TT.find("linux") == std::string::npos)
    IsLinux = false;

  // Is the target big endian?
  if (TT.find("eb") == std::string::npos)
    IsLittle = false;
}
