//===-- Nios2SelectionDAGInfo.h - Nios2 SelectionDAG Info ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the Nios2 subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef MIPSSELECTIONDAGINFO_H
#define MIPSSELECTIONDAGINFO_H

#include "llvm/Target/TargetSelectionDAGInfo.h"

namespace llvm {

class Nios2TargetMachine;

class Nios2SelectionDAGInfo : public TargetSelectionDAGInfo {
public:
  explicit Nios2SelectionDAGInfo(const Nios2TargetMachine &TM);
  ~Nios2SelectionDAGInfo();
};

}

#endif
