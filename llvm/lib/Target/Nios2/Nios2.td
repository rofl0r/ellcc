//===- Nios2.td - Describe the Nios2 Target Machine ---------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This is the top level entry point for the Nios2 target.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "Nios2RegisterInfo.td"
include "Nios2Schedule.td"
include "Nios2InstrInfo.td"
include "Nios2CallingConv.td"

def Nios2InstrInfo : InstrInfo;

def FeatureMul    : SubtargetFeature<"mul", "HasMul", "true",
                               "Enable integer multiply instruction.">;
def FeatureMulx   : SubtargetFeature<"mulx", "HasMulx", "true",
                               "Enable extendewd integer multiply instruction.">;
def FeatureDiv    : SubtargetFeature<"div", "HasDiv", "true",
                               "Enable integer divide instruction.">;

//===----------------------------------------------------------------------===//
// Nios2 processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, Nios2GenericItineraries, Features>;

def : Proc<"nios2", []>;

def Nios2 : Target {
  let InstructionSet = Nios2InstrInfo;
}
