#ifndef _RAISE_INSTRUCTIONS_H_
#define _RAISE_INSTRUCTIONS_H_

#include "TargetInfo.h"

namespace llvm {
class FunctionType;
class Pass;
class PassInfo;
};

namespace ellcc {
//===----------------------------------------------------------------------===//
//
// RaiseInstructions - Turn unsupported target instructions into function calls.
//
// TODO: If TargetData will supply a list, this struct will move.
llvm::Pass *createRaiseInstructionsPass(const TargetInfo::RaiseInstructionsList* List = 0, int Size = 0);
extern const llvm::PassInfo* const RaiseInstructionsID;
} // End ellcc namespace

#endif
