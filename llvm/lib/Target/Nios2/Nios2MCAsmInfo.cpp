//===-- Nios2MCAsmInfo.cpp - Nios2 asm properties -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the Nios2MCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "Nios2MCAsmInfo.h"
using namespace llvm;

Nios2MCAsmInfo::Nios2MCAsmInfo(const Target &T, const StringRef &TT)
{
        AlignmentIsInBytes          = false;
        Data16bitsDirective         = "\t.half\t";
        Data32bitsDirective         = "\t.word\t";
        Data64bitsDirective         = 0;
        PrivateGlobalPrefix         = "$";
        CommentString               = "#";
        ZeroDirective               = "\t.space\t";
}
