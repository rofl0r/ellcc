//===- Nios2RegisterInfo.h - Nios2 Register Information Impl ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Nios2 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef NIOS2REGISTERINFO_H
#define NIOS2REGISTERINFO_H

#include "Nios2.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "Nios2GenRegisterInfo.h.inc"

namespace llvm {
class Nios2Subtarget;
class TargetInstrInfo;
class Type;

struct Nios2RegisterInfo : public Nios2GenRegisterInfo {
  const Nios2Subtarget &Subtarget;
  const TargetInstrInfo &TII;
  
  Nios2RegisterInfo(const Nios2Subtarget &Subtarget, const TargetInstrInfo &tii);

  /// getRegisterNumbering - Given the enum value for some register, e.g.
  /// Nios2::RA, return the number that it corresponds to (e.g. 31).
  static unsigned getRegisterNumbering(unsigned RegEnum);

  /// Get PIC indirect call register
  static unsigned getPICCallReg(); 

  /// Adjust the Nios2 stack frame.
  void adjustNios2StackFrame(MachineFunction &MF) const;

  /// Code Generation virtual methods...
  const unsigned *getCalleeSavedRegs(const MachineFunction* MF = 0) const;

  const TargetRegisterClass* const*
  getCalleeSavedRegClasses(const MachineFunction* MF = 0) const;

  BitVector getReservedRegs(const MachineFunction &MF) const;

  bool hasFP(const MachineFunction &MF) const;

  void eliminateCallFramePseudoInstr(MachineFunction &MF,
                                     MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I) const;

  /// Stack Frame Processing Methods
  unsigned eliminateFrameIndex(MachineBasicBlock::iterator II,
                               int SPAdj, FrameIndexValue *Value = NULL,
                               RegScavenger *RS = NULL) const;

  void processFunctionBeforeFrameFinalized(MachineFunction &MF) const;

  void emitPrologue(MachineFunction &MF) const;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const;
  
  /// Debug information queries.
  unsigned getRARegister() const;
  unsigned getFrameRegister(const MachineFunction &MF) const;

  /// Exception handling queries.
  unsigned getEHExceptionRegister() const;
  unsigned getEHHandlerRegister() const;

  int getDwarfRegNum(unsigned RegNum, bool isEH) const;
};

} // end namespace llvm

#endif
