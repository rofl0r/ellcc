//===-- Nios2AsmPrinter.cpp - Nios2 LLVM assembly writer --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format Nios2 assembly language.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "nios2-asm-printer"
#include "Nios2.h"
#include "Nios2Subtarget.h"
#include "Nios2InstrInfo.h"
#include "Nios2TargetMachine.h"
#include "Nios2MachineFunction.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Target/Mangler.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
  class Nios2AsmPrinter : public AsmPrinter {

    const Nios2Subtarget *Subtarget;
  public:
    explicit Nios2AsmPrinter(TargetMachine &TM,  MCStreamer &Streamer)
      : AsmPrinter(TM, Streamer) {
      Subtarget = &TM.getSubtarget<Nios2Subtarget>();
    }

    virtual const char *getPassName() const {
      return "Nios2 Assembly Printer";
    }

    bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo, 
                         unsigned AsmVariant, const char *ExtraCode,
                         raw_ostream &O);
    void printOperand(const MachineInstr *MI, int opNum, raw_ostream &O);
    void printUnsignedImm(const MachineInstr *MI, int opNum, raw_ostream &O);
    void printMemOperand(const MachineInstr *MI, int opNum, raw_ostream &O,
                         const char *Modifier = 0);

    void printInstruction(const MachineInstr *MI, raw_ostream &O);  // autogen'd.
    void EmitInstruction(const MachineInstr *MI) {
      SmallString<128> Str;
      raw_svector_ostream OS(Str);
      printInstruction(MI, OS);
      OutStreamer.EmitRawText(OS.str());
    }
    virtual void EmitFunctionBodyStart();
    virtual void EmitFunctionBodyEnd();
    static const char *getRegisterName(unsigned RegNo);
    virtual void EmitFunctionEntryLabel();
    void EmitStartOfAsmFile(Module &M);
  };
} // end of anonymous namespace

#include "Nios2GenAsmWriter.inc"

//===----------------------------------------------------------------------===//
// Frame and Set directives
//===----------------------------------------------------------------------===//

void Nios2AsmPrinter::EmitFunctionEntryLabel() {
  OutStreamer.EmitRawText("\t.ent\t" + Twine(CurrentFnSym->getName()));
  OutStreamer.EmitLabel(CurrentFnSym);
}

/// EmitFunctionBodyStart - Targets can override this to emit stuff before
/// the first basic block in the function.
void Nios2AsmPrinter::EmitFunctionBodyStart() {
}

/// EmitFunctionBodyEnd - Targets can override this to emit stuff after
/// the last basic block in the function.
void Nios2AsmPrinter::EmitFunctionBodyEnd() {
  OutStreamer.EmitRawText("\t.end\t" + Twine(CurrentFnSym->getName()));
}

// Print out an operand for an inline asm expression.
bool Nios2AsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo, 
                                     unsigned AsmVariant,const char *ExtraCode,
                                     raw_ostream &O) {
  // Does this asm operand have a single letter operand modifier?
  if (ExtraCode && ExtraCode[0]) 
    return true; // Unknown modifier.

  printOperand(MI, OpNo, O);
  return false;
}

void Nios2AsmPrinter::printOperand(const MachineInstr *MI, int opNum,
                                   raw_ostream &O) {
  const MachineOperand &MO = MI->getOperand(opNum);
  bool closeP = false;

  if (MO.getTargetFlags())
    closeP = true;


  switch(MO.getTargetFlags()) {
  case Nios2II::MO_GPREL:    O << "%gp_rel("; break;
  case Nios2II::MO_GOT_CALL: O << "%call16("; break;
  case Nios2II::MO_GOT:
    if (MI->getOpcode() == Nios2::LDW)
      O << "%got(";
      O << "%lo(";
    break;
  case Nios2II::MO_ABS_HILO:
    if (MI->getOpcode() == Nios2::MOVHi)
      O << "%hi(";
    else
      O << "%lo(";     
    break;
  }

#if RICH        // May need some of this.
  // %hi and %lo used on nios2 gas to load global addresses on
  // static code. %got is used to load global addresses when 
  // using PIC_. %call16 is used to load direct call targets
  // on PIC_ and small code size. %call_lo and %call_hi load 
  // direct call targets on PIC_ and large code size.
  if (MI->getOpcode() == Nios2::MOVHi && !MO.isReg() 
      && !MO.isImm()) {
    if ((isPIC) && (isCodeLarge))
      O << "%call_hi(";
    else
      O << "%hi(";

  } else if ((MI->getOpcode() == Nios2::ADDi || MI->getOpcode() == Nios2::ORi)
             && !MO.isReg() && !MO.isImm()) {
    O << "%lo(";
    closeP = true;
  } else if ((isPIC) && (MI->getOpcode() == Nios2::LDW)
             && (!MO.isReg()) && (!MO.isImm())) {
    const MachineOperand &firstMO = MI->getOperand(opNum-1);
    const MachineOperand &lastMO  = MI->getOperand(opNum+1);
    if ((firstMO.isReg()) && (lastMO.isReg())) {
      if ((firstMO.getReg() == Nios2::T7) && (lastMO.getReg() == Nios2::GP) 
          && (!isCodeLarge))
        O << "%call16(";
      else if ((firstMO.getReg() != Nios2::T7) && (lastMO.getReg() == Nios2::GP))
        O << "%got(";
      else if ((firstMO.getReg() == Nios2::T7) && (lastMO.getReg() != Nios2::GP) 
               && (isCodeLarge))
        O << "%call_lo(";
      closeP = true;
    }
  }
#endif
 
  switch (MO.getType()) {
    case MachineOperand::MO_Register:
      O << '$' << LowercaseString(getRegisterName(MO.getReg()));
      break;

    case MachineOperand::MO_Immediate:
      O << (short int)MO.getImm();
      break;

    case MachineOperand::MO_MachineBasicBlock:
      O << *MO.getMBB()->getSymbol();
      return;

    case MachineOperand::MO_GlobalAddress:
      O << *Mang->getSymbol(MO.getGlobal());
      break;

    case MachineOperand::MO_ExternalSymbol:
      O << *GetExternalSymbolSymbol(MO.getSymbolName());
      break;

    case MachineOperand::MO_JumpTableIndex:
      O << MAI->getPrivateGlobalPrefix() << "JTI" << getFunctionNumber()
        << '_' << MO.getIndex();
      break;

    case MachineOperand::MO_ConstantPoolIndex:
      O << MAI->getPrivateGlobalPrefix() << "CPI"
        << getFunctionNumber() << "_" << MO.getIndex();
      if (MO.getOffset())
        O << "+" << MO.getOffset();
      break;
  
    default:
      llvm_unreachable("<unknown operand type>");
  }

  if (closeP) O << ")";
}

void Nios2AsmPrinter::printUnsignedImm(const MachineInstr *MI, int opNum,
                                       raw_ostream &O) {
  const MachineOperand &MO = MI->getOperand(opNum);
  if (MO.isImm())
    O << (unsigned short int)MO.getImm();
  else 
    printOperand(MI, opNum, O);
}

void Nios2AsmPrinter::
printMemOperand(const MachineInstr *MI, int opNum, raw_ostream &O,
                const char *Modifier) {
  // when using stack locations for not load/store instructions
  // print the same way as all normal 3 operand instructions.
  if (Modifier && !strcmp(Modifier, "stackloc")) {
    printOperand(MI, opNum+1, O);
    O << ", ";
    printOperand(MI, opNum, O);
    return;
  }

  // Load/Store memory operands -- imm(reg) 
  // If PIC target the target is loaded as the 
  // pattern lw r25,%call16(r28)
  printOperand(MI, opNum, O);
  O << "(";
  printOperand(MI, opNum+1, O);
  O << ")";
}

void Nios2AsmPrinter::EmitStartOfAsmFile(Module &M) 
{
}

// Force static initialization.
extern "C" void LLVMInitializeNios2AsmPrinter() {
  RegisterAsmPrinter<Nios2AsmPrinter> X(TheNios2Target);
  RegisterAsmPrinter<Nios2AsmPrinter> Y(TheNios2ebTarget);
}
