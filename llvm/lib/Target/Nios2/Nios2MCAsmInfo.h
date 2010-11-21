//=====-- Nios2MCAsmInfo.h - Nios2 asm properties -----------*- C++ -*--====//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the Nios2MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef NIOS2TARGETASMINFO_H
#define NIOS2TARGETASMINFO_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {
  class Target;
  class StringRef;

  class Nios2MCAsmInfo : public MCAsmInfo {
  public:
    explicit Nios2MCAsmInfo(const Target &T, const StringRef &TT);
  };
} // namespace llvm

#endif
