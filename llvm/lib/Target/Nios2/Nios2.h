//===-- Nios2.h - Top-level interface for Nios2 representation ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in 
// the LLVM Nios2 back-end.
//
//===----------------------------------------------------------------------===//

#ifndef TARGET_NIOS2_H
#define TARGET_NIOS2_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class Nios2TargetMachine;
  class FunctionPass;
  class MachineCodeEmitter;
  class formatted_raw_ostream;

  FunctionPass *createNios2ISelDag(Nios2TargetMachine &TM);
  extern Target TheNios2Target;
  extern Target TheNios2ebTarget;
} // end namespace llvm;

// Defines symbolic names for Nios2 registers.  This defines a mapping from
// register name to register number.
#include "Nios2GenRegisterNames.inc"

// Defines symbolic names for the Nios2 instructions.
#include "Nios2GenInstrNames.inc"

#endif
