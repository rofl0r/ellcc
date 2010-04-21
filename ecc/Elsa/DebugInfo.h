//===--- DebugInfo.h - DebugInfo for LLVM CodeGen -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is the source level debug info generator for llvm translation. 
//
//===----------------------------------------------------------------------===//

#ifndef _DEBUG_INFO_H_
#define _DEBUG_INFO_H_

#include "cc_type.h"
#include "cc2llvm.h"
#include "SourceLocation.h"
#include "LangOptions.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Analysis/DebugInfo.h"
#include "llvm/Support/IRBuilder.h"
#include <llvm/Support/TargetFolder.h>
#include "llvm/Support/ValueHandle.h"
#include "llvm/Support/Allocator.h"
#include <map>

class CC2LLVMEnv;
namespace ellcc {

typedef llvm::IRBuilder<true, llvm::TargetFolder> BuilderTy;

/// DebugInfo - This class gathers all debug information during compilation 
/// and is responsible for emitting to llvm globals or pass directly to 
/// the backend.
class DebugInfo {
  CC2LLVMEnv& env;                  
  LangOptions& LO;
  bool isMainCompileUnitCreated;
  llvm::DIFactory DebugFactory;
  llvm::DICompileUnit TheCU;
  SourceLocation CurLoc, PrevLoc;

  /// CompileUnitCache - Cache of previously constructed CompileUnits.
  llvm::DenseMap<unsigned, llvm::DIFile> CompileUnitCache;

  /// TypeCache - Cache of previously constructed Types.
  llvm::DenseMap<void *, llvm::WeakVH> TypeCache;
  
  bool BlockLiteralGenericSet;
  llvm::DIType BlockLiteralGeneric;

  std::vector<llvm::TrackingVH<llvm::MDNode> > RegionStack;
  llvm::DenseMap<const void *, llvm::WeakVH> RegionMap;

  llvm::DenseMap<const char *, llvm::WeakVH> DIFileCache;

  /// Helper functions for getOrCreateType.
  llvm::DIType CreateType(const SimpleType *Ty, llvm::DIFile U);
  llvm::DIType CreateCVRType(const CVAtomicType* Ty, CVFlags cv, llvm::DIFile U);
  llvm::DIType CreateType(const PointerType *Ty, llvm::DIFile U);
  llvm::DIType CreateType(const FunctionType *Ty, llvm::DIFile U);
  // RICH: llvm::DIType CreateType(const TagType *Ty, llvm::DIFile U);
  llvm::DIType CreateType(const CompoundType *Ty, llvm::DIFile U);
  llvm::DIType CreateType(const EnumType *Ty, llvm::DIFile U);
  llvm::DIType CreateType(const ArrayType *Ty, llvm::DIFile U);

public:
  DebugInfo(CC2LLVMEnv& env, LangOptions& LO);
  ~DebugInfo();

  llvm::DIDescriptor getContext(const Variable *Decl, llvm::DIDescriptor &CompileUnit);
  /// setLocation - Update the current source location. If \arg loc is
  /// invalid it is ignored.
  void setLocation(SourceLocation Loc);

  /// EmitStopPoint - Emit a call to llvm.dbg.stoppoint to indicate a change of
  /// source line.
  void EmitStopPoint(llvm::Function *Fn, BuilderTy &Builder);

  /// EmitFunctionStart - Emit a call to llvm.dbg.function.start to indicate
  /// start of a new function.
  void EmitFunctionStart(const char *Name, Type* ReturnType,
                         llvm::Function *Fn, BuilderTy &Builder);
  
  /// EmitRegionStart - Emit a call to llvm.dbg.region.start to indicate start
  /// of a new block.  
  void EmitRegionStart(llvm::Function *Fn, BuilderTy &Builder);
  
  /// EmitRegionEnd - Emit call to llvm.dbg.region.end to indicate end of a 
  /// block.
  void EmitRegionEnd(llvm::Function *Fn, BuilderTy &Builder);

  /// EmitDeclareOfAutoVariable - Emit call to llvm.dbg.declare for an automatic
  /// variable declaration.
  void EmitDeclareOfAutoVariable(const Variable *Decl, llvm::Value *AI,
                                 BuilderTy &Builder);

  /// EmitDeclareOfArgVariable - Emit call to llvm.dbg.declare for an argument
  /// variable declaration.
  void EmitDeclareOfArgVariable(const Variable *Decl, llvm::Value *AI,
                                BuilderTy &Builder);
  
  /// EmitGlobalVariable - Emit information about a global variable.
  void EmitGlobalVariable(llvm::GlobalVariable *GV, const Variable *Decl);

private:
  /// EmitDeclare - Emit call to llvm.dbg.declare for a variable declaration.
  void EmitDeclare(const Variable *decl, unsigned Tag, llvm::Value *AI,
                   BuilderTy &Builder);
  
  /// getOrCreateFile - Get the compile unit from the cache or create a
  /// new one if necessary.
  llvm::DIFile getOrCreateFile(SourceLocation Loc);
  void CreateCompileUnit();

  /// getOrCreateType - Get the type from the cache or create a new type if
  /// necessary.
  llvm::DIType getOrCreateType(const Type* Ty, llvm::DIFile Unit, bool cvDone = false);
};
} // namespace ellcc

#endif
