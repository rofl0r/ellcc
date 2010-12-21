//===- Nios2InstrInfo.h - Nios2 Instruction Information -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Nios2 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef NIOS2INSTRUCTIONINFO_H
#define NIOS2INSTRUCTIONINFO_H

#include "Nios2.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "Nios2RegisterInfo.h"

namespace llvm {

namespace Nios2 {

  // Nios2 Branch Codes
  enum FPBranchCode {
    BRANCH_F,
    BRANCH_T,
    BRANCH_FL,
    BRANCH_TL,
    BRANCH_INVALID
  };

  // Nios2 Condition Codes
  enum CondCode {
    COND_E,
    COND_NE,
    COND_GE,
    COND_GT,
    COND_LE,
    COND_LT,
    COND_INVALID
  };
  
  // Turn condition code into conditional branch opcode.
  unsigned GetCondBranchFromCond(CondCode CC);

  /// GetOppositeBranchCondition - Return the inverse of the specified cond,
  /// e.g. turning COND_E to COND_NE.
  CondCode GetOppositeBranchCondition(Nios2::CondCode CC);
}

/// MipsII - This namespace holds all of the target specific flags that
/// instruction info tracks.
///
namespace Nios2II {
  /// Target Operand Flag enum.
  enum TOF {
    //===------------------------------------------------------------------===//
    // Nios2 Specific MachineOperand flags.
 
    MO_NO_FLAG,

    /// MO_GOT - Represents the offset into the global offset table at which
    /// the address the relocation entry symbol resides during execution.
    MO_GOT,

    /// MO_GOT_CALL - Represents the offset into the global offset table at 
    /// which the address of a call site relocation entry symbol resides 
    /// during execution. This is different from the above since this flag
    /// can only be present in call instructions.
    MO_GOT_CALL,

    /// MO_GPREL - Represents the offset from the current gp value to be used 
    /// for the relocatable object file being produced.
    MO_GPREL,

    /// MO_ABS_HILO - Represents the hi or low part of an absolute symbol
    /// address. 
    MO_ABS_HILO

  };
}

class Nios2InstrInfo : public TargetInstrInfoImpl {
  Nios2TargetMachine &TM;
  const Nios2RegisterInfo RI;
public:
  explicit Nios2InstrInfo(Nios2TargetMachine &TM);

  /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
  /// such, whenever a client has an instance of instruction info, it should
  /// always be able to get register info as well (through this method).
  ///
  virtual const Nios2RegisterInfo &getRegisterInfo() const { return RI; }

  /// Return true if the instruction is a register to register move and return
  /// the source and dest operands and their sub-register indices by reference.
  virtual bool isMoveInstr(const MachineInstr &MI,
                           unsigned &SrcReg, unsigned &DstReg,
                           unsigned &SrcSubIdx, unsigned &DstSubIdx) const;
  
  /// isLoadFromStackSlot - If the specified machine instruction is a direct
  /// load from a stack slot, return the virtual or physical register number of
  /// the destination along with the FrameIndex of the loaded stack slot.  If
  /// not, return 0.  This predicate must return 0 if the instruction has
  /// any side effects other than loading from the stack slot.
  virtual unsigned isLoadFromStackSlot(const MachineInstr *MI,
                                       int &FrameIndex) const;
  
  /// isStoreToStackSlot - If the specified machine instruction is a direct
  /// store to a stack slot, return the virtual or physical register number of
  /// the source reg along with the FrameIndex of the loaded stack slot.  If
  /// not, return 0.  This predicate must return 0 if the instruction has
  /// any side effects other than storing to the stack slot.
  virtual unsigned isStoreToStackSlot(const MachineInstr *MI,
                                      int &FrameIndex) const;
 
  /// Branch Analysis
  virtual bool AnalyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                             MachineBasicBlock *&FBB,
                             SmallVectorImpl<MachineOperand> &Cond,
                             bool AllowModify) const;
  virtual unsigned RemoveBranch(MachineBasicBlock &MBB) const;
  virtual unsigned InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                                MachineBasicBlock *FBB,
                            const SmallVectorImpl<MachineOperand> &Cond) const;
  virtual bool copyRegToReg(MachineBasicBlock &MBB, 
                            MachineBasicBlock::iterator I,
                            unsigned DestReg, unsigned SrcReg,
                            const TargetRegisterClass *DestRC,
                            const TargetRegisterClass *SrcRC,
                            DebugLoc DL) const;
  virtual void storeRegToStackSlot(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator MBBI,
                                   unsigned SrcReg, bool isKill, int FrameIndex,
                                   const TargetRegisterClass *RC,
                                   const TargetRegisterInfo *TRI) const;

  virtual void loadRegFromStackSlot(MachineBasicBlock &MBB,
                                    MachineBasicBlock::iterator MBBI,
                                    unsigned DestReg, int FrameIndex,
                                    const TargetRegisterClass *RC,
                                    const TargetRegisterInfo *TRI) const;

  virtual MachineInstr* foldMemoryOperandImpl(MachineFunction &MF,
                                              MachineInstr* MI,
                                              const SmallVectorImpl<unsigned> &Ops,
                                              int FrameIndex) const;

  virtual MachineInstr* foldMemoryOperandImpl(MachineFunction &MF,
                                              MachineInstr* MI,
                                              const SmallVectorImpl<unsigned> &Ops,
                                              MachineInstr* LoadMI) const {
    return 0;
  }
  
  virtual
  bool ReverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const;

  /// Insert nop instruction when hazard condition is found
  virtual void insertNoop(MachineBasicBlock &MBB, 
                          MachineBasicBlock::iterator MI) const;

  /// getGlobalBaseReg - Return a virtual register initialized with the
  /// the global base register value. Output instructions required to
  /// initialize the register in the function entry block, if necessary.
  ///
  unsigned getGlobalBaseReg(MachineFunction *MF) const;
};

}

#endif
