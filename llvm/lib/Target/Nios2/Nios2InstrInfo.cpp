//===- Nios2InstrInfo.cpp - Nios2 Instruction Information ---------*- C++ -*-===//
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

#include "Nios2InstrInfo.h"
#include "Nios2TargetMachine.h"
#include "Nios2MachineFunction.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "Nios2GenInstrInfo.inc"

using namespace llvm;

Nios2InstrInfo::Nios2InstrInfo(Nios2TargetMachine &tm)
  : TargetInstrInfoImpl(Nios2Insts, array_lengthof(Nios2Insts)),
    TM(tm), RI(*TM.getSubtargetImpl(), *this) {}

static bool isZeroImm(const MachineOperand &op) {
  return op.isImm() && op.getImm() == 0;
}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the destination along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than loading from the stack slot.
unsigned Nios2InstrInfo::
isLoadFromStackSlot(const MachineInstr *MI, int &FrameIndex) const 
{
  if (MI->getOpcode() == Nios2::LDW) {
    if ((MI->getOperand(2).isFI()) && // is a stack slot
        (MI->getOperand(1).isImm()) &&  // the imm is zero
        (isZeroImm(MI->getOperand(1)))) {
      FrameIndex = MI->getOperand(2).getIndex();
      return MI->getOperand(0).getReg();
    }
  }

  return 0;
}

/// isStoreToStackSlot - If the specified machine instruction is a direct
/// store to a stack slot, return the virtual or physical register number of
/// the source reg along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
unsigned Nios2InstrInfo::
isStoreToStackSlot(const MachineInstr *MI, int &FrameIndex) const 
{
  if (MI->getOpcode() == Nios2::STW) {
    if ((MI->getOperand(2).isFI()) && // is a stack slot
        (MI->getOperand(1).isImm()) &&  // the imm is zero
        (isZeroImm(MI->getOperand(1)))) {
      FrameIndex = MI->getOperand(2).getIndex();
      return MI->getOperand(0).getReg();
    }
  }
  return 0;
}

/// insertNoop - If data hazard condition is found insert the target nop
/// instruction.
void Nios2InstrInfo::
insertNoop(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI) const 
{
  DebugLoc dl;
  BuildMI(MBB, MI, dl, get(Nios2::NOP));
}

bool Nios2InstrInfo::
copyRegToReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
             unsigned DestReg, unsigned SrcReg,
             const TargetRegisterClass *DestRC,
             const TargetRegisterClass *SrcRC,
	     DebugLoc DL) const {
  if (DestRC != SrcRC) {
    return false;
  }

  if (DestRC == Nios2::CPURegsRegisterClass)
    BuildMI(MBB, I, DL, get(Nios2::MOV), DestReg).addReg(SrcReg);
  else
      return false;     // Can't copy this register
  return true;
}

void Nios2InstrInfo::
storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
          unsigned SrcReg, bool isKill, int FI, 
          const TargetRegisterClass *RC,
          const TargetRegisterInfo *TRI) const 
{
  DebugLoc dl;
  if (I != MBB.end()) dl = I->getDebugLoc();

  if (RC == Nios2::CPURegsRegisterClass) {
    BuildMI(MBB, I, dl, get(Nios2::STW)).addReg(SrcReg, getKillRegState(isKill))
            .addImm(0).addFrameIndex(FI);
  } else {
    llvm_unreachable("Register class not handled!");
  }
}

void Nios2InstrInfo::
loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                     unsigned DestReg, int FI,
                     const TargetRegisterClass *RC,
                     const TargetRegisterInfo *TRI) const 
{
  DebugLoc dl;
  if (I != MBB.end()) dl = I->getDebugLoc();
  if (RC == Nios2::CPURegsRegisterClass) {
    BuildMI(MBB, I, dl, get(Nios2::LDW), DestReg).addImm(0).addFrameIndex(FI);
  } else {
    llvm_unreachable("Register class not handled!");
  }
}

MachineInstr *Nios2InstrInfo::
foldMemoryOperandImpl(MachineFunction &MF,
                  MachineInstr* MI,
                  const SmallVectorImpl<unsigned> &Ops, int FI) const 
{
  if (Ops.size() != 1) return NULL;

  MachineInstr *NewMI = NULL;
  DebugLoc dl = MI->getDebugLoc();

  bool doLoad = false;
  switch (MI->getOpcode()) {
  case Nios2::MOV:
      if (Ops[0] == 0) {    // COPY -> STORE
        unsigned SrcReg = MI->getOperand(1).getReg();
        bool isKill = MI->getOperand(1).isKill();
        bool isUndef = MI->getOperand(1).isUndef();
        NewMI = BuildMI(MF, dl, get(Nios2::STW))
          .addReg(SrcReg, getKillRegState(isKill) | getUndefRegState(isUndef))
          .addImm(0).addFrameIndex(FI);
      } else {              // COPY -> LOAD
        doLoad = true;
      }
      break;
  case Nios2::ADD:
  case Nios2::OR:
    if ((MI->getOperand(0).isReg()) &&
        (MI->getOperand(1).isReg()) && 
        (MI->getOperand(1).getReg() == Nios2::ZERO) &&
        (MI->getOperand(2).isReg())) {
      if (Ops[0] == 0) {    // COPY -> STORE
        unsigned SrcReg = MI->getOperand(2).getReg();
        bool isKill = MI->getOperand(2).isKill();
        bool isUndef = MI->getOperand(2).isUndef();
        NewMI = BuildMI(MF, dl, get(Nios2::STW))
                .addReg(SrcReg, getKillRegState(isKill) | getUndefRegState(isUndef))
                .addImm(0).addFrameIndex(FI);
      } else {              // COPY -> LOAD
        doLoad = true;
      }
    } else if ((MI->getOperand(0).isReg()) &&
               (MI->getOperand(2).isReg()) && 
               (MI->getOperand(2).getReg() == Nios2::ZERO) &&
               (MI->getOperand(1).isReg())) {
      if (Ops[0] == 0) {    // COPY -> STORE
        unsigned SrcReg = MI->getOperand(1).getReg();
        bool isKill = MI->getOperand(1).isKill();
        bool isUndef = MI->getOperand(1).isUndef();
        NewMI = BuildMI(MF, dl, get(Nios2::STW))
          .addReg(SrcReg, getKillRegState(isKill) | getUndefRegState(isUndef))
          .addImm(0).addFrameIndex(FI);
      } else {              // COPY -> LOAD
        doLoad = true;
      }
    }
    break;
  }

  if (doLoad) {
    unsigned DstReg = MI->getOperand(0).getReg();
    bool isDead = MI->getOperand(0).isDead();
    bool isUndef = MI->getOperand(0).isUndef();
    NewMI = BuildMI(MF, dl, get(Nios2::LDW))
      .addReg(DstReg, true, getDeadRegState(isDead) | getUndefRegState(isUndef))
      .addImm(0).addFrameIndex(FI);
  }
  return NewMI;
}

//===----------------------------------------------------------------------===//
// Branch Analysis
//===----------------------------------------------------------------------===//

/// GetCondFromBranchOpc - Return the Nios2 CC that matches 
/// the correspondent Branch instruction opcode.
static Nios2::CondCode GetCondFromBranchOpc(unsigned BrOpc) 
{
  switch (BrOpc) {
  default: return Nios2::COND_INVALID;
  case Nios2::BEQ  : return Nios2::COND_E;
  case Nios2::BNE  : return Nios2::COND_NE;
  case Nios2::BGE  : return Nios2::COND_GE;
  case Nios2::BGT  : return Nios2::COND_GT;
  case Nios2::BLE  : return Nios2::COND_LE;
  case Nios2::BLT  : return Nios2::COND_LT;
  }
}

/// GetCondBranchFromCond - Return the Branch instruction
/// opcode that matches the cc.
unsigned Nios2::GetCondBranchFromCond(Nios2::CondCode CC) 
{
  switch (CC) {
  default: llvm_unreachable("Illegal condition code!");
  case Nios2::COND_E   : return Nios2::BEQ;
  case Nios2::COND_NE  : return Nios2::BNE;
  case Nios2::COND_GE  : return Nios2::BGE;
  case Nios2::COND_GT  : return Nios2::BGT;
  case Nios2::COND_LE  : return Nios2::BLE;
  case Nios2::COND_LT  : return Nios2::BLT;
  }
}

/// GetOppositeBranchCondition - Return the inverse of the specified 
/// condition, e.g. turning COND_E to COND_NE.
Nios2::CondCode Nios2::GetOppositeBranchCondition(Nios2::CondCode CC) 
{
  switch (CC) {
  default: llvm_unreachable("Illegal condition code!");
  case Nios2::COND_E   : return Nios2::COND_NE;
  case Nios2::COND_NE  : return Nios2::COND_E;
  case Nios2::COND_GE  : return Nios2::COND_LT;
  case Nios2::COND_GT  : return Nios2::COND_LE;
  case Nios2::COND_LE  : return Nios2::COND_GT;
  case Nios2::COND_LT  : return Nios2::COND_GE;
  }
}

bool Nios2InstrInfo::AnalyzeBranch(MachineBasicBlock &MBB, 
                                  MachineBasicBlock *&TBB,
                                  MachineBasicBlock *&FBB,
                                  SmallVectorImpl<MachineOperand> &Cond,
                                  bool AllowModify) const 
{
  // If the block has no terminators, it just falls into the block after it.
  MachineBasicBlock::iterator I = MBB.end();
  if (I == MBB.begin())
    return false;
  --I;
  while (I->isDebugValue()) {
    if (I == MBB.begin())
      return false;
    --I;
  }
  if (!isUnpredicatedTerminator(I))
    return false;
  
  // Get the last instruction in the block.
  MachineInstr *LastInst = I;
  
  // If there is only one terminator instruction, process it.
  unsigned LastOpc = LastInst->getOpcode();
  if (I == MBB.begin() || !isUnpredicatedTerminator(--I)) {
    if (!LastInst->getDesc().isBranch())
      return true;

    // Unconditional branch
    if (LastOpc == Nios2::JMPi || LastOpc == Nios2::BR) {
      TBB = LastInst->getOperand(0).getMBB();
      return false;
    }

    Nios2::CondCode BranchCode = GetCondFromBranchOpc(LastInst->getOpcode());
    if (BranchCode == Nios2::COND_INVALID)
      return true;  // Can't handle indirect branch.

    // Conditional branch
    // Block ends with fall-through condbranch.
    if (LastOpc != Nios2::COND_INVALID) {
      int LastNumOp = LastInst->getNumOperands();

      TBB = LastInst->getOperand(LastNumOp-1).getMBB();
      Cond.push_back(MachineOperand::CreateImm(BranchCode));

      for (int i=0; i<LastNumOp-1; i++) {
        Cond.push_back(LastInst->getOperand(i));
      }

      return false;
    }
  }
  
  // Get the instruction before it if it is a terminator.
  MachineInstr *SecondLastInst = I;
  
  // If there are three terminators, we don't know what sort of block this is.
  if (SecondLastInst && I != MBB.begin() && isUnpredicatedTerminator(--I))
    return true;

  // If the block ends with Nios2::JMPi and a Nios2::BNE/Nios2::BEQ, handle it.
  unsigned SecondLastOpc    = SecondLastInst->getOpcode();
  Nios2::CondCode BranchCode = GetCondFromBranchOpc(SecondLastOpc);

  if (BranchCode != Nios2::COND_INVALID && (LastOpc == Nios2::JMPi || LastOpc == Nios2::BR)) {
    int SecondNumOp = SecondLastInst->getNumOperands();

    TBB = SecondLastInst->getOperand(SecondNumOp-1).getMBB();
    Cond.push_back(MachineOperand::CreateImm(BranchCode));

    for (int i=0; i<SecondNumOp-1; i++) {
      Cond.push_back(SecondLastInst->getOperand(i));
    }

    FBB = LastInst->getOperand(0).getMBB();
    return false;
  }
  
  // If the block ends with two unconditional branches, handle it. The last 
  // one is not executed, so remove it.
  if ((SecondLastOpc == Nios2::JMPi || SecondLastOpc == Nios2::BR) &&
      (LastOpc == Nios2::JMPi || LastOpc == Nios2::BR)) {
    TBB = SecondLastInst->getOperand(0).getMBB();
    I = LastInst;
    I->eraseFromParent();
    return false;
  }

  // Otherwise, can't handle this.
  return true;
}

unsigned Nios2InstrInfo::
InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB, 
             MachineBasicBlock *FBB,
             const SmallVectorImpl<MachineOperand> &Cond) const {
  // FIXME this should probably have a DebugLoc argument
  DebugLoc dl;
  // Shouldn't be a fall through.
  assert(TBB && "InsertBranch must not be told to insert a fallthrough");
  assert((Cond.size() == 3 || Cond.size() == 2 || Cond.size() == 0) &&
         "Nios2 branch conditions can have two|three components!");

  if (FBB == 0) { // One way branch.
    if (Cond.empty()) {
      // Unconditional branch?
      BuildMI(&MBB, dl, get(Nios2::JMPi)).addMBB(TBB);
    } else {
      // Conditional branch.
      unsigned Opc = GetCondBranchFromCond((Nios2::CondCode)Cond[0].getImm());
      const TargetInstrDesc &TID = get(Opc);

      if (TID.getNumOperands() == 3)
        BuildMI(&MBB, dl, TID).addReg(Cond[1].getReg())
                          .addReg(Cond[2].getReg())
                          .addMBB(TBB);
      else
        BuildMI(&MBB, dl, TID).addReg(Cond[1].getReg())
                          .addMBB(TBB);

    }                             
    return 1;
  }
  
  // Two-way Conditional branch.
  unsigned Opc = GetCondBranchFromCond((Nios2::CondCode)Cond[0].getImm());
  const TargetInstrDesc &TID = get(Opc);

  if (TID.getNumOperands() == 3)
    BuildMI(&MBB, dl, TID).addReg(Cond[1].getReg()).addReg(Cond[2].getReg())
                      .addMBB(TBB);
  else
    BuildMI(&MBB, dl, TID).addReg(Cond[1].getReg()).addMBB(TBB);

  BuildMI(&MBB, dl, get(Nios2::JMPi)).addMBB(FBB);
  return 2;
}

unsigned Nios2InstrInfo::
RemoveBranch(MachineBasicBlock &MBB) const 
{
  MachineBasicBlock::iterator I = MBB.end();
  if (I == MBB.begin()) return 0;
  --I;
  while (I->isDebugValue()) {
    if (I == MBB.begin())
      return 0;
    --I;
  }
  if (I->getOpcode() != Nios2::JMPi &&
      I->getOpcode() != Nios2::BR && 
      GetCondFromBranchOpc(I->getOpcode()) == Nios2::COND_INVALID)
    return 0;
  
  // Remove the branch.
  I->eraseFromParent();
  
  I = MBB.end();
  
  if (I == MBB.begin()) return 1;
  --I;
  if (GetCondFromBranchOpc(I->getOpcode()) == Nios2::COND_INVALID)
    return 1;
  
  // Remove the branch.
  I->eraseFromParent();
  return 2;
}

/// ReverseBranchCondition - Return the inverse opcode of the 
/// specified Branch instruction.
bool Nios2InstrInfo::
ReverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const 
{
  assert( (Cond.size() == 3 || Cond.size() == 2) && 
          "Invalid Nios2 branch condition!");
  Cond[0].setImm(GetOppositeBranchCondition((Nios2::CondCode)Cond[0].getImm()));
  return false;
}

/// getGlobalBaseReg - Return a virtual register initialized with the
/// the global base register value. Output instructions required to
/// initialize the register in the function entry block, if necessary.
///
unsigned Nios2InstrInfo::getGlobalBaseReg(MachineFunction *MF) const {
  Nios2FunctionInfo *Nios2FI = MF->getInfo<Nios2FunctionInfo>();
  unsigned GlobalBaseReg = Nios2FI->getGlobalBaseReg();
  if (GlobalBaseReg != 0)
    return GlobalBaseReg;

  // Insert the set of GlobalBaseReg into the first MBB of the function
  MachineBasicBlock &FirstMBB = MF->front();
  MachineBasicBlock::iterator MBBI = FirstMBB.begin();
  MachineRegisterInfo &RegInfo = MF->getRegInfo();
  const TargetInstrInfo *TII = MF->getTarget().getInstrInfo();

  GlobalBaseReg = RegInfo.createVirtualRegister(Nios2::CPURegsRegisterClass);
  bool Ok = TII->copyRegToReg(FirstMBB, MBBI, GlobalBaseReg, Nios2::GP,
                              Nios2::CPURegsRegisterClass,
                              Nios2::CPURegsRegisterClass,
			      DebugLoc());
  assert(Ok && "Couldn't assign to global base register!");
  Ok = Ok; // Silence warning when assertions are turned off.
  RegInfo.addLiveIn(Nios2::GP);

  Nios2FI->setGlobalBaseReg(GlobalBaseReg);
  return GlobalBaseReg;
}
