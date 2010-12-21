//===-- Nios2SelectionDAGInfo.cpp - Nios2 SelectionDAG Info -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Nios2SelectionDAGInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "nios2-selectiondag-info"
#include "Nios2TargetMachine.h"
using namespace llvm;

Nios2SelectionDAGInfo::Nios2SelectionDAGInfo(const Nios2TargetMachine &TM)
  : TargetSelectionDAGInfo(TM) {
}

Nios2SelectionDAGInfo::~Nios2SelectionDAGInfo() {
}
