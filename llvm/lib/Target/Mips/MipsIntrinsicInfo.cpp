//===- MipsIntrinsicInfo.cpp - Intrinsic Information -00-------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Mips implementation of TargetIntrinsicInfo.
//
//===----------------------------------------------------------------------===//

#include "MipsIntrinsicInfo.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
#include "llvm/Intrinsics.h"
#include "llvm/Module.h"
#include "llvm/Type.h"
#include "llvm/Support/raw_ostream.h"
#include <cstring>

using namespace llvm;

namespace mipsIntrinsic {

  enum ID {
    last_non_mips_intrinsic = Intrinsic::num_intrinsics-1,
#define GET_INTRINSIC_ENUM_VALUES
#include "MipsGenIntrinsics.inc"
#undef GET_INTRINSIC_ENUM_VALUES
    , num_mips_intrinsics
  };

#define GET_LLVM_INTRINSIC_FOR_GCC_BUILTIN
#include "MipsGenIntrinsics.inc"
#undef GET_LLVM_INTRINSIC_FOR_GCC_BUILTIN
}

std::string MipsIntrinsicInfo::getName(unsigned IntrID, const Type **Tys,
                                         unsigned numTys) const {
  static const char *const names[] = {
#define GET_INTRINSIC_NAME_TABLE
#include "MipsGenIntrinsics.inc"
#undef GET_INTRINSIC_NAME_TABLE
  };

  assert(!isOverloaded(IntrID) && "Mips intrinsics are not overloaded");
  if (IntrID < Intrinsic::num_intrinsics)
    return 0;
  assert(IntrID < mipsIntrinsic::num_mips_intrinsics &&
         "Invalid intrinsic ID");

  std::string Result(names[IntrID - Intrinsic::num_intrinsics]);
  return Result;
}

unsigned MipsIntrinsicInfo::
lookupName(const char *Name, unsigned Len) const {
  if (Len < 5 || Name[4] != '.' || Name[0] != 'l' || Name[1] != 'l'
      || Name[2] != 'v' || Name[3] != 'm')
    return 0;  // All intrinsics start with 'llvm.'

#define GET_FUNCTION_RECOGNIZER
#include "MipsGenIntrinsics.inc"
#undef GET_FUNCTION_RECOGNIZER
  return 0;
}

unsigned MipsIntrinsicInfo::
lookupGCCName(const char *Name) const {
    return mipsIntrinsic::getIntrinsicForGCCBuiltin("mips",Name);
}

bool MipsIntrinsicInfo::isOverloaded(unsigned IntrID) const {
  // Overload Table
  const bool OTable[] = {
#define GET_INTRINSIC_OVERLOAD_TABLE
#include "MipsGenIntrinsics.inc"
#undef GET_INTRINSIC_OVERLOAD_TABLE
  };
  if (IntrID == 0)
    return false;
  else
    return OTable[IntrID - Intrinsic::num_intrinsics];
}

/// This defines the "getAttributes(ID id)" method.
#define GET_INTRINSIC_ATTRIBUTES
#include "MipsGenIntrinsics.inc"
#undef GET_INTRINSIC_ATTRIBUTES

static const FunctionType *getType(LLVMContext &Context, unsigned id) {
  const Type *ResultTy = NULL;
  std::vector<const Type*> ArgTys;
  bool IsVarArg = false;

#define GET_INTRINSIC_GENERATOR
#include "MipsGenIntrinsics.inc"
#undef GET_INTRINSIC_GENERATOR

  return FunctionType::get(ResultTy, ArgTys, IsVarArg);
}

Function *MipsIntrinsicInfo::getDeclaration(Module *M, unsigned IntrID,
                                                const Type **Tys,
                                                unsigned numTy) const {
  assert(!isOverloaded(IntrID) && "Mips intrinsics are not overloaded");
  AttrListPtr AList = getAttributes((mipsIntrinsic::ID) IntrID);
  return cast<Function>(M->getOrInsertFunction(getName(IntrID),
                                               getType(M->getContext(), IntrID),
                                               AList));
}
