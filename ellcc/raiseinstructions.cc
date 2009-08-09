//===- RaiseInstructions.cpp - Raise unsupported insts to calls -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// The RaiseInstructions transformation is a target-dependent tranformation
// because it depends on the target's support of certain data types.
//
// Each instruction can map to one or more functions. We look at the
// function signature and the instruction's operands and result type
// to match instructions to functions. This allows, e.g., umul to
// map to i32 umuli32(i32, i32) and i64 umuli64(i64, i64).
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "raiseinstructions"
#include "raiseinstructions.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "llvm/Module.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Constants.h"
#include "llvm/LLVMContext.h"
#include "llvm/Pass.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Support/Compiler.h"
#include <map>
using namespace llvm;
using namespace ellcc;

STATISTIC(NumRaised, "Number of instructions raised");

namespace {
  /// RaiseInstructions - Turn unsupported insts into library calls.
  ///
  class VISIBILITY_HIDDEN RaiseInstructions : public BasicBlockPass {
    const TargetInfo::RaiseInstructionsList* List;      // The input list.
    int Size;                                           // Length of the list.
    typedef std::multimap<short, const TargetInfo::RaiseInstructionsList*> MapType;
    MapType Map;                                        // The lookup map.
    Module* M;                                          // The containing module.
  public:
    static char ID; // Pass ID, replacement for typeid
    explicit RaiseInstructions(const TargetInfo::RaiseInstructionsList* List = 0,
                               int Size = 0)
      : BasicBlockPass(&ID), List(List), Size(Size), M(0) {}

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.addRequired<TargetData>();
      AU.setPreservesCFG();

      // This is a cluster of orthogonal Transforms:
      AU.addPreserved<UnifyFunctionExitNodes>();
      AU.addPreservedID(PromoteMemoryToRegisterID);
      AU.addPreservedID(LowerSwitchID);
      AU.addPreservedID(LowerInvokePassID);
    }

    /// doPassInitialization - For the raise insrtuctions pass, this
    /// creates the instruction map.
    ///
    bool doInitialization(Module &Mod);

    virtual bool doInitialization(Function &F) {
      return doInitialization(*F.getParent());
    }

    /// runOnBasicBlock - This method does the actual work of converting
    /// instructions over, assuming that the pass has already been initialized.
    ///
    bool runOnBasicBlock(BasicBlock &BB);
  };
}

char RaiseInstructions::ID = 0;
static RegisterPass<RaiseInstructions>
X("raiseinstructions", "Raise instructions to calls");

// Publically exposed interface to pass...
const PassInfo *const ellcc::RaiseInstructionsID = &X;
// createRaiseInstructionsPass - Interface to this file...
Pass *ellcc::createRaiseInstructionsPass(const TargetInfo::RaiseInstructionsList* List,
                                         int Size) {
  return new RaiseInstructions(List, Size);
}


// doInitialization - For the raise instruction pass, this creates
// a map of instructions to functions.
//
// This function is always successful.
//
bool RaiseInstructions::doInitialization(Module &Mod) {
  for (int Index = 0; Index < Size; ++Index) {
    Map.insert(MapType::value_type(List[Index].Instruction, &List[Index]));
  }

  M = &Mod;
  return true;
}

// runOnBasicBlock - This method does the actual work of converting
// instructions over, assuming that the pass has already been initialized.
//
bool RaiseInstructions::runOnBasicBlock(BasicBlock &BB) {
  // TODO: If TD is used to get the function list:
  // const TargetData &TD = getAnalysis<TargetData>();

  assert(M && "RaiseInstructions pass not initialized!");
  if (!List) {
    // TODO: Check TargetData for an instruction list (or in doInitialization?).
    return false;
  }

  bool Changed = false;
  BasicBlock::InstListType &BBIL = BB.getInstList();

  // Loop over all of the instructions, looking for instructions that match.
  for (BasicBlock::iterator I = BB.begin(), E = BB.end(); I != E; ++I) {
    // Find all the possible matches for this instruction.
    std::pair<MapType::iterator, MapType::iterator> Matches
        = Map.equal_range(I->getOpcode());
    for ( ; Matches.first != Matches.second; ++Matches.first) {
      // Have a match, compare the result type to the function return type.
      assert((*Matches.first).second->FuncType
             && "RaiseInstructions entry missing FuncType!");
      FunctionType* FT = (*Matches.first).second->FuncType;
      if (FT->getReturnType() != I->getType()) {
        continue;
      }
      // OK, now compare the number of operands to parameters.
      if (FT->getNumParams() != I->getNumOperands()) {
        continue;
      }
      // Finally, all the operand types must match the parameter types.
      bool NoMatch = false;
      for (unsigned Index = 0; Index < I->getNumOperands(); ++Index) {
        if (I->getOperand(Index)->getType() != FT->getParamType(Index)) {
          NoMatch = true;
          break;
        }
      }
      if (NoMatch) {
        // We had a parameter mismatch.
        continue;
      }

      // Everything matches. Replace the instruction with the call.
      Constant *Func
        = M->getOrInsertFunction((*Matches.first).second->Name, FT);
      // Build the argument list.
      std::vector<Value*> Args;
      for (unsigned Index = 0; Index < I->getNumOperands(); ++Index) {
        Args.push_back(I->getOperand(Index));
      }
      // Insert a call to the function...
      CallInst* Call = CallInst::Create(Func, Args.begin(), Args.end(), "", I);
      I->replaceAllUsesWith(Call);
      // Delete the old instruction.
      I = --BBIL.erase(I);
      Changed = true;
      ++NumRaised;
    }
  }

  return Changed;
}

