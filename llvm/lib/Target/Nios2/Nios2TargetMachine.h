//===-- Nios2TargetMachine.h - Define TargetMachine for Nios2 -00--*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Nios2 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef NIOS2TARGETMACHINE_H
#define NIOS2TARGETMACHINE_H

#include "Nios2Subtarget.h"
#include "Nios2InstrInfo.h"
#include "Nios2ISelLowering.h"
#include "Nios2SelectionDAGInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetFrameInfo.h"

namespace llvm {
  class formatted_raw_ostream;

  class Nios2TargetMachine : public LLVMTargetMachine {
    Nios2Subtarget      Subtarget;
    const TargetData    DataLayout; // Calculates type size & alignment
    Nios2InstrInfo      InstrInfo;
    TargetFrameInfo     FrameInfo;
    Nios2TargetLowering TLInfo;
    Nios2SelectionDAGInfo TSInfo;

  public:
    Nios2TargetMachine(const Target& T, const std::string &TT,
                       const std::string &FS, bool isLittle);

    virtual const Nios2InstrInfo   *getInstrInfo()     const 
    { return &InstrInfo; }
    virtual const TargetFrameInfo *getFrameInfo()     const 
    { return &FrameInfo; }
    virtual const Nios2Subtarget   *getSubtargetImpl() const 
    { return &Subtarget; }
    virtual const TargetData      *getTargetData()    const 
    { return &DataLayout;}

    virtual const Nios2RegisterInfo *getRegisterInfo()  const {
      return &InstrInfo.getRegisterInfo();
    }

    virtual const Nios2TargetLowering *getTargetLowering() const { 
      return &TLInfo; 
    }

    virtual const Nios2SelectionDAGInfo* getSelectionDAGInfo() const {
      return &TSInfo;
    }

    // Pass Pipeline Configuration
    virtual bool addInstSelector(PassManagerBase &PM,
                                 CodeGenOpt::Level OptLevel);
    virtual bool addPreEmitPass(PassManagerBase &PM,
                                CodeGenOpt::Level OptLevel);
  };

/// Nios2ebTargetMachine - Nios2eb target machine.
///
class Nios2ebTargetMachine : public Nios2TargetMachine {
public:
  Nios2ebTargetMachine(const Target& T, const std::string &TT,
                       const std::string &FS);
};

} // End llvm namespace

#endif
