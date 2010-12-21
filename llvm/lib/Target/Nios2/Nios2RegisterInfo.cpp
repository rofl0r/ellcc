//===- Nios2RegisterInfo.cpp - NIOS2 Register Information -== -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the NIOS2 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "nios2-reg-info"

#include "Nios2.h"
#include "Nios2Subtarget.h"
#include "Nios2RegisterInfo.h"
#include "Nios2MachineFunction.h"
#include "llvm/Constants.h"
#include "llvm/Type.h"
#include "llvm/Function.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineLocation.h"
#include "llvm/Target/TargetFrameInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"

using namespace llvm;

Nios2RegisterInfo::Nios2RegisterInfo(const Nios2Subtarget &ST, 
        const TargetInstrInfo &tii)
: Nios2GenRegisterInfo(Nios2::ADJCALLSTACKDOWN, Nios2::ADJCALLSTACKUP),
    Subtarget(ST), TII(tii) {}

    /// getRegisterNumbering - Given the enum value for some register, e.g.
    /// Nios2::RA, return the number that it corresponds to (e.g. 31).
    unsigned Nios2RegisterInfo::
getRegisterNumbering(unsigned RegEnum) 
{
    switch (RegEnum) {
        case Nios2::ZERO : return 0;
        case Nios2::AT   : return 1;
        case Nios2::V0   : return 2;
        case Nios2::V1   : return 3;
        case Nios2::A0   : return 4;
        case Nios2::A1   : return 5;
        case Nios2::A2   : return 6;
        case Nios2::A3   : return 7;
        case Nios2::T0   : return 8;
        case Nios2::T1   : return 9;
        case Nios2::T2   : return 10;
        case Nios2::T3   : return 11;
        case Nios2::T4   : return 12;
        case Nios2::T5   : return 13;
        case Nios2::T6   : return 14;
        case Nios2::T7   : return 15;
        case Nios2::S0   : return 16;
        case Nios2::S1   : return 17;
        case Nios2::S2   : return 18;
        case Nios2::S3   : return 19;
        case Nios2::S4   : return 20;
        case Nios2::S5   : return 21;
        case Nios2::S6   : return 22;
        case Nios2::S7   : return 23;
        case Nios2::ET   : return 24;
        case Nios2::BT   : return 25;
        case Nios2::GP   : return 26;
        case Nios2::SP   : return 27;
        case Nios2::FP   : return 28;
        case Nios2::EA   : return 29;
        case Nios2::BA   : return 30;
        case Nios2::RA   : return 31;
        default: llvm_unreachable("Unknown register number!");
    }    
    return 0; // Not reached
}

unsigned Nios2RegisterInfo::getPICCallReg() { return Nios2::T7; }

//===----------------------------------------------------------------------===//
// Callee Saved Registers methods 
//===----------------------------------------------------------------------===//

/// Nios2 Callee Saved Registers
const unsigned* Nios2RegisterInfo::
getCalleeSavedRegs(const MachineFunction *MF) const 
{
    // Nios2 callee-save registers

    static const unsigned BitMode32CalleeSavedRegs[] = {
        Nios2::S0, Nios2::S1, Nios2::S2, Nios2::S3, 
        Nios2::S4, Nios2::S5, Nios2::S6, Nios2::S7, 0
    };

    return BitMode32CalleeSavedRegs;
}

/// Nios2 Callee Saved Register Classes
const TargetRegisterClass* const* 
Nios2RegisterInfo::getCalleeSavedRegClasses(const MachineFunction *MF) const 
{
    static const TargetRegisterClass * const BitMode32CalleeSavedRC[] = {
        &Nios2::CPURegsRegClass, &Nios2::CPURegsRegClass, &Nios2::CPURegsRegClass, 
        &Nios2::CPURegsRegClass, &Nios2::CPURegsRegClass, &Nios2::CPURegsRegClass,
        &Nios2::CPURegsRegClass, &Nios2::CPURegsRegClass, 0
    };

    return BitMode32CalleeSavedRC;
}

BitVector Nios2RegisterInfo::
getReservedRegs(const MachineFunction &MF) const
{
    BitVector Reserved(getNumRegs());
    Reserved.set(Nios2::ZERO);
    Reserved.set(Nios2::AT);
    Reserved.set(Nios2::ET);
    Reserved.set(Nios2::BT);
    Reserved.set(Nios2::GP);
    Reserved.set(Nios2::SP);
    if (hasFP(MF)) {
        Reserved.set(Nios2::FP);
    }
    Reserved.set(Nios2::EA);
    Reserved.set(Nios2::BA);
    Reserved.set(Nios2::RA);
    return Reserved;
}

//===----------------------------------------------------------------------===//
//
// Stack Frame Processing methods
// +----------------------------+
//
// The stack is allocated decrementing the stack pointer on
// the first instruction of a function prologue. Once decremented,
// all stack references are are done thought a positive offset
// from the stack/frame pointer, so the stack is considering
// to grow up! Otherwise terrible hacks would have to be made
// to get this stack ABI compliant :)
//
//  The stack frame required by the ABI (after call):
//  Offset
//
//  0                 ----------
//  4                 Args to pass 
//  .                 Alloca allocations
//  .                 Local Area
//  .                 CPU "Callee Saved" Registers
//  .                 saved FP
//  .                 saved RA
//  StackSize         -----------
//
// Offset - offset from sp after stack allocation on function prologue
//
// The sp is the stack pointer subtracted/added from the stack size
// at the Prologue/Epilogue
//
// References to the previous stack (to obtain arguments) are done
// with offsets that exceeds the stack size: (stacksize+(4*(num_arg-1))
//
// Examples:
// - reference to the actual stack frame
//   for any local area var there is smt like : FI >= 0, StackOffset: 4
//     sw REGX, 4(SP)
//
// - reference to previous stack frame
//   suppose there's a load to the 5th arguments : FI < 0, StackOffset: 16.
//   The emitted instruction will be something like:
//     lw REGX, 16+StackSize(SP)
//
// Since the total stack size is unknown on LowerFormalArguments, all
// stack references (ObjectOffset) created to reference the function 
// arguments, are negative numbers. This way, on eliminateFrameIndex it's
// possible to detect those references and the offsets are adjusted to
// their real location.
//
//===----------------------------------------------------------------------===//

void Nios2RegisterInfo::adjustNios2StackFrame(MachineFunction &MF) const
{
    MachineFrameInfo *MFI = MF.getFrameInfo();
    Nios2FunctionInfo *Nios2FI = MF.getInfo<Nios2FunctionInfo>();
    const std::vector<CalleeSavedInfo> &CSI = MFI->getCalleeSavedInfo();
    unsigned StackAlign = MF.getTarget().getFrameInfo()->getStackAlignment();
    unsigned RegSize = 4;
    bool HasGP = Nios2FI->needGPSaveRestore();

    // Min and Max CSI FrameIndex.
    int MinCSFI = -1, MaxCSFI = -1; 

    // See the description at Nios2MachineFunction.h
    int TopCPUSavedRegOff = -1;

    // It happens that the default stack frame allocation order does not directly 
    // map to the convention used for nios2. So we must fix it. We move the callee 
    // save register slots after the local variables area, as described in the
    // stack frame above.
    unsigned CalleeSavedAreaSize = 0;
    if (!CSI.empty()) {
        MinCSFI = CSI[0].getFrameIdx();
        MaxCSFI = CSI[CSI.size()-1].getFrameIdx();
    }
    for (unsigned i = 0, e = CSI.size(); i != e; ++i)
        CalleeSavedAreaSize += MFI->getObjectAlignment(CSI[i].getFrameIdx());

  unsigned StackOffset = HasGP ? (Nios2FI->getGPStackOffset()+RegSize) : 16;

    // Adjust local variables. They should come on the stack right
    // after the arguments.
    int LastOffsetFI = -1;
    for (int i = 0, e = MFI->getObjectIndexEnd(); i != e; ++i) {
        if (i >= MinCSFI && i <= MaxCSFI)
            continue;
        if (MFI->isDeadObjectIndex(i))
            continue;
        unsigned Offset =
            StackOffset + MFI->getObjectOffset(i) - CalleeSavedAreaSize;
        if (LastOffsetFI == -1)
            LastOffsetFI = i;
        if (Offset > MFI->getObjectOffset(LastOffsetFI))
            LastOffsetFI = i;
        MFI->setObjectOffset(i, Offset);
    }

    // Adjust CPU Callee Saved Registers Area. Registers RA and FP must
    // be saved in this CPU Area. This whole Area must be aligned to the
    // default Stack Alignment requirements.

    if (LastOffsetFI >= 0)
        StackOffset = MFI->getObjectOffset(LastOffsetFI)+ 
                      MFI->getObjectSize(LastOffsetFI);
    StackOffset = ((StackOffset+StackAlign-1)/StackAlign*StackAlign);

    int stackSize  = MF.getFrameInfo()->getStackSize();
    if (MFI->hasCalls()) {
        stackSize += RegSize;
    }

    if (hasFP(MF)) {
        stackSize += RegSize;
    }
    
    for (unsigned i = 0, e = CSI.size(); i != e ; ++i) {
        if (CSI[i].getRegClass() != Nios2::CPURegsRegisterClass)
            break;
        stackSize += RegSize;
    }

    if (MFI->hasCalls()) {
        MFI->setObjectOffset(MFI->CreateStackObject(RegSize, RegSize, true),
                             hasFP(MF) ? 4 : 0);
        Nios2FI->setRAStackOffset(hasFP(MF) ? 4 : 0);
        TopCPUSavedRegOff = hasFP(MF) ? 4 : 0;
        StackOffset += RegSize;
    }

    if (hasFP(MF)) {
        MFI->setObjectOffset(MFI->CreateStackObject(RegSize, RegSize, true), 0);
        Nios2FI->setFPStackOffset(0);
        TopCPUSavedRegOff = 0;
        StackOffset += RegSize;
    }

    for (unsigned i = 0, e = CSI.size(); i != e ; ++i) {
        if (CSI[i].getRegClass() != Nios2::CPURegsRegisterClass)
            break;
        MFI->setObjectOffset(CSI[i].getFrameIdx(), stackSize + (-(StackOffset + 4)));
        TopCPUSavedRegOff = stackSize + (-(StackOffset + 4));
        StackOffset += MFI->getObjectAlignment(CSI[i].getFrameIdx());
    }

    // Update frame info
    MFI->setStackSize(stackSize);
}

// hasFP - Return true if the specified function should have a dedicated frame
// pointer register.  This is true if the function has variable sized allocas or
// if frame pointer elimination is disabled.
bool Nios2RegisterInfo::
hasFP(const MachineFunction &MF) const {
    return (DisableFramePointerElim(MF) || MF.getFrameInfo()->hasVarSizedObjects());
}

// This function eliminate ADJCALLSTACKDOWN, 
// ADJCALLSTACKUP pseudo instructions
void Nios2RegisterInfo::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
        MachineBasicBlock::iterator I) const {
    // Simply discard ADJCALLSTACKDOWN, ADJCALLSTACKUP instructions.
    MBB.erase(I);
}

// FrameIndex represent objects inside a abstract stack.
// We must replace FrameIndex with an stack/frame pointer
// direct reference.
unsigned Nios2RegisterInfo::
eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, 
                    FrameIndexValue *Value, RegScavenger *RS) const 
{
    MachineInstr &MI = *II;
    MachineFunction &MF = *MI.getParent()->getParent();

    unsigned i = 0;
    while (!MI.getOperand(i).isFI()) {
        ++i;
        assert(i < MI.getNumOperands() && 
                "Instr doesn't have FrameIndex operand!");
    }

    DEBUG(errs() << "\nFunction : " << MF.getFunction()->getName() << "\n";
          errs() << "<--------->\n" << MI);

    int FrameIndex = MI.getOperand(i).getIndex();
    int stackSize  = MF.getFrameInfo()->getStackSize();
    int spOffset   = MF.getFrameInfo()->getObjectOffset(FrameIndex);

    DEBUG(errs() << "FrameIndex : " << FrameIndex << "\n"
                 << "spOffset   : " << spOffset << "\n"
                 << "stackSize  : " << stackSize << "\n");

    // as explained on LowerFormalArguments, detect negative offsets 
    // and adjust SPOffsets considering the final stack size.
    int Offset = ((spOffset < 0) ? (stackSize + (-(spOffset + 4)))
                                 : (stackSize + (-(spOffset + 4))));
    Offset    += MI.getOperand(i-1).getImm();

    DEBUG(errs() << "Offset     : " << Offset << "\n" << "<--------->\n");

    MI.getOperand(i-1).ChangeToImmediate(Offset);
    MI.getOperand(i).ChangeToRegister(getFrameRegister(MF), false);
    return 0;
}

void Nios2RegisterInfo::
emitPrologue(MachineFunction &MF) const
{
    MachineBasicBlock &MBB = MF.front();
    MachineBasicBlock::iterator MBBI = MBB.begin();
    DebugLoc dl = (MBBI != MBB.end() ?  MBBI->getDebugLoc() : DebugLoc());
    MachineFrameInfo  *MFI = MF.getFrameInfo();
    Nios2FunctionInfo *Nios2FI = MF.getInfo<Nios2FunctionInfo>();
    adjustNios2StackFrame(MF);
    // Get the number of bytes to allocate from the FrameInfo.
    unsigned StackSize = MFI->getStackSize();
    if (StackSize == 0) {
        return;
    }

    // Adjust stack : addi sp, sp, (-imm)
    BuildMI(MBB, MBBI, dl, TII.get(Nios2::ADDi), Nios2::SP)
        .addReg(Nios2::SP).addImm(-StackSize);

    // Save the return address only if the function isnt a leaf one.
    // stw  $ra, stack_loc($sp)
    if (MFI->hasCalls()) { 
        int RAOffset = Nios2FI->getRAStackOffset();
        BuildMI(MBB, MBBI, dl, TII.get(Nios2::STW))
            .addReg(Nios2::RA).addImm(RAOffset).addReg(Nios2::SP);
    }

    // if framepointer enabled, save it and set it
    // to point to the stack pointer
    if (hasFP(MF)) {
        // stw  $fp,stack_loc($sp)
        int FPOffset = Nios2FI->getFPStackOffset();
        BuildMI(MBB, MBBI, dl, TII.get(Nios2::STW))
            .addReg(Nios2::FP).addImm(FPOffset).addReg(Nios2::SP);

        // move $fp, $sp
        BuildMI(MBB, MBBI, dl, TII.get(Nios2::MOV), Nios2::FP)
            .addReg(Nios2::SP);
    }
}

void Nios2RegisterInfo::
emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const 
{
    MachineBasicBlock::iterator MBBI = prior(MBB.end());
    MachineFrameInfo *MFI = MF.getFrameInfo();
    Nios2FunctionInfo *Nios2FI = MF.getInfo<Nios2FunctionInfo>();
    DebugLoc dl = MBBI->getDebugLoc();

    // Get the number of bytes from FrameInfo
    int NumBytes = (int) MFI->getStackSize();
    if (NumBytes == 0) {
        return;
    }
     
    // Get the FI's where RA and FP are saved.

    // if framepointer enabled, restore it and restore the
    // stack pointer
    if (hasFP(MF)) {
        // mov $sp, $fp
        BuildMI(MBB, MBBI, dl, TII.get(Nios2::MOV), Nios2::SP)
            .addReg(Nios2::FP);

        int FPOffset = Nios2FI->getFPStackOffset();
        // ldw  $fp,stack_loc($sp)
        BuildMI(MBB, MBBI, dl, TII.get(Nios2::LDW))
            .addReg(Nios2::FP).addImm(FPOffset).addReg(Nios2::SP);
    }

    // Restore the return address only if the function isn't a leaf one.
    // ldw  $ra, stack_loc($sp)
    if (MFI->hasCalls()) { 
        int RAOffset = Nios2FI->getRAStackOffset();
        BuildMI(MBB, MBBI, dl, TII.get(Nios2::LDW))
            .addReg(Nios2::RA).addImm(RAOffset).addReg(Nios2::SP);
    }

    // adjust stack  : insert addi sp, sp, (imm)
    if (NumBytes) {
        BuildMI(MBB, MBBI, dl, TII.get(Nios2::ADDi), Nios2::SP)
            .addReg(Nios2::SP).addImm(NumBytes);
    }
}

void Nios2RegisterInfo::
processFunctionBeforeFrameFinalized(MachineFunction &MF) const {
    // Set the stack offset where GP must be saved/loaded from.
    MachineFrameInfo *MFI = MF.getFrameInfo();
    Nios2FunctionInfo *Nios2FI = MF.getInfo<Nios2FunctionInfo>();
    if (Nios2FI->needGPSaveRestore()) {
        MFI->setObjectOffset(Nios2FI->getGPFI(), Nios2FI->getGPStackOffset());
    }    
}

unsigned Nios2RegisterInfo::
getRARegister() const {
    return Nios2::RA;
}

unsigned Nios2RegisterInfo::
getFrameRegister(const MachineFunction &MF) const {
    return hasFP(MF) ? Nios2::FP : Nios2::SP;
}

unsigned Nios2RegisterInfo::
getEHExceptionRegister() const {
    llvm_unreachable("What is the exception register");
    return 0;
}

unsigned Nios2RegisterInfo::
getEHHandlerRegister() const {
    llvm_unreachable("What is the exception handler register");
    return 0;
}

int Nios2RegisterInfo::
getDwarfRegNum(unsigned RegNum, bool isEH) const {
    llvm_unreachable("What is the dwarf register number");
    return -1;
}

#include "Nios2GenRegisterInfo.inc"

