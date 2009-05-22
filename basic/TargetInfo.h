//===--- TargetInfo.h - Expose information about the target -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the TargetInfo interface.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_BASIC_TARGETINFO_H
#define LLVM_CLANG_BASIC_TARGETINFO_H

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#define __STDC_CONSTANT_MACROS 1
#endif
#include "llvm/Support/DataTypes.h"
#include "llvm/ADT/StringMap.h"
#include <vector>
#include <string>

namespace llvm { struct fltSemantics; }

namespace ellcc {

class Diagnostic;
class SourceManager;
class LangOptions;
  
namespace Builtin { struct Info; }
  
/// TargetInfo - This class exposes information about the current target.
///
class TargetInfo {
  std::string Triple;
protected:
  // Target values set by the ctor of the actual target implementation.  Default
  // values are specified by the TargetInfo constructor.
  bool BigEndian;
  bool CharIsSigned;
  unsigned char CharWidth, CharAlign, PrefCharAlign;
  unsigned char WCharWidth, WCharAlign, PrefWCharAlign;
  unsigned char BoolWidth, BoolAlign, PrefBoolAlign;
  unsigned char ShortWidth, ShortAlign, PrefShortAlign;
  unsigned char IntWidth, IntAlign, PrefIntAlign;
  unsigned char FloatWidth, FloatAlign, PrefFloatAlign;
  unsigned char DoubleWidth, DoubleAlign, PrefDoubleAlign;
  unsigned char LongDoubleWidth, LongDoubleAlign, PrefLongDoubleAlign;
  unsigned char LongWidth, LongAlign, PrefLongAlign;
  unsigned char LongLongWidth, LongLongAlign, PrefLongLongAlign;
  unsigned char PointerWidth, PointerAlign, PrefPointerAlign;
  unsigned char VectorWidth, VectorAlign, PrefVectorAlign;
  unsigned char LongVectorWidth, LongVectorAlign, PrefLongVectorAlign;
  unsigned char AggregateWidth, AggregateAlign, PrefAggregateAlign;

  unsigned char IntMaxTWidth;
  const char *UserLabelPrefix;
  const llvm::fltSemantics *FloatFormat, *DoubleFormat, *LongDoubleFormat;

  // TargetInfo Constructor.  Default initializes all fields.
  TargetInfo(const std::string &T);
  
public:  
  /// CreateTargetInfo - Return the target info object for the specified target
  /// triple.
  static TargetInfo* CreateTargetInfo(const std::string &Triple);

  virtual ~TargetInfo();

  ///===---- Target Data Type Query Methods -------------------------------===//
  enum IntType {
    NoInt = 0,
    SignedShort,
    UnsignedShort,
    SignedInt,
    UnsignedInt,
    SignedLong,
    UnsignedLong,
    SignedLongLong,
    UnsignedLongLong
  };
protected:
  IntType SizeType, IntMaxType, UIntMaxType, PtrDiffType, IntPtrType, WCharType;
public:
  IntType getSizeType() const { return SizeType; }
  IntType getIntMaxType() const { return IntMaxType; }
  IntType getUIntMaxType() const { return UIntMaxType; }
  IntType getPtrDiffType(unsigned AddrSpace) const {
    return AddrSpace == 0 ? PtrDiffType : getPtrDiffTypeV(AddrSpace);
  }
  IntType getIntPtrType() const { return IntPtrType; }
  IntType getWCharType() const { return WCharType; }

  /// isCharSigned - Return true if 'char' is 'signed char' or false if it is
  /// treated as 'unsigned char'.  This is implementation defined according to
  /// C99 6.2.5p15.  In our implementation, this is target-specific.
  bool isCharSigned() const { return CharIsSigned; }
  
  /// getPointerWidth - Return the width of pointers on this target, for the
  /// specified address space.
  uint64_t getPointerWidth(unsigned AddrSpace) const {
    return AddrSpace == 0 ? PointerWidth : getPointerWidthV(AddrSpace);
  }
  uint64_t getPointerAlign(unsigned AddrSpace) const {
    return AddrSpace == 0 ? PointerAlign : getPointerAlignV(AddrSpace);
  }
  
  /// getBoolWidth/Align - Return the size of '_Bool' and C++ 'bool' for this
  /// target, in bits.
  unsigned getBoolWidth() const { return BoolWidth; }
  unsigned getBoolAlign() const { return BoolAlign; }
  
  unsigned getCharWidth(bool isWide = false) const {
    return isWide ? getWCharWidth() : CharWidth;
  }
  unsigned getCharAlign(bool isWide = false) const {
    return isWide ? getWCharAlign() : CharAlign;
  }
  
  /// getShortWidth/Align - Return the size of 'signed short' and
  /// 'unsigned short' for this target, in bits.  
  unsigned getShortWidth() const { return ShortWidth; }
  unsigned getShortAlign() const { return ShortAlign; }
  
  /// getIntWidth/Align - Return the size of 'signed int' and 'unsigned int' for
  /// this target, in bits.
  unsigned getIntWidth() const { return IntWidth; }
  unsigned getIntAlign() const { return IntAlign; }
  
  /// getLongWidth/Align - Return the size of 'signed long' and 'unsigned long'
  /// for this target, in bits.
  unsigned getLongWidth() const { return LongWidth; }
  unsigned getLongAlign() const { return LongAlign; }
  
  /// getLongLongWidth/Align - Return the size of 'signed long long' and
  /// 'unsigned long long' for this target, in bits.
  unsigned getLongLongWidth() const { return LongLongWidth; }
  unsigned getLongLongAlign() const { return LongLongAlign; }
  
  /// getWcharWidth/Align - Return the size of 'wchar_t' for this target, in
  /// bits.
  unsigned getWCharWidth() const { return WCharWidth; }
  unsigned getWCharAlign() const { return WCharAlign; }

  /// getFloatWidth/Align/Format - Return the size/align/format of 'float'.
  unsigned getFloatWidth() const { return FloatWidth; }
  unsigned getFloatAlign() const { return FloatAlign; }
  const llvm::fltSemantics &getFloatFormat() const { return *FloatFormat; }

  /// getDoubleWidth/Align/Format - Return the size/align/format of 'double'.
  unsigned getDoubleWidth() const { return DoubleWidth; }
  unsigned getDoubleAlign() const { return DoubleAlign; }
  const llvm::fltSemantics &getDoubleFormat() const { return *DoubleFormat; }

  /// getLongDoubleWidth/Align/Format - Return the size/align/format of 'long
  /// double'.
  unsigned getLongDoubleWidth() const { return LongDoubleWidth; }
  unsigned getLongDoubleAlign() const { return LongDoubleAlign; }
  const llvm::fltSemantics &getLongDoubleFormat() const {
    return *LongDoubleFormat;
  }
  
  /// getIntMaxTWidth - Return the size of intmax_t and uintmax_t for this
  /// target, in bits.  
  unsigned getIntMaxTWidth() const {
    return IntMaxTWidth;
  }
  
  /// getUserLabelPrefix - This returns the default value of the
  /// __USER_LABEL_PREFIX__ macro, which is the prefix given to user symbols by
  /// default.  On most platforms this is "_", but it is "" on some, and "." on
  /// others.
  const char *getUserLabelPrefix() const {
    return UserLabelPrefix;
  }
  
  /// getTypeName - Return the user string for the specified integer type enum.
  /// For example, SignedShort -> "short".
  static const char *getTypeName(IntType T);
  
  ///===---- Other target property query methods --------------------------===//
  
  /// getTargetDefines - Appends the target-specific #define values for this
  /// target set to the specified buffer.
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &DefineBuffer) const = 0;
  
  /// getTargetBuiltins - Return information about target-specific builtins for
  /// the current primary target, and info about which builtins are non-portable
  /// across the current set of primary and secondary targets.
  virtual void getTargetBuiltins(const Builtin::Info *&Records, 
                                 unsigned &NumRecords) const = 0;

  /// getVAListDeclaration - Return the declaration to use for
  /// __builtin_va_list, which is target-specific.
  virtual const char *getVAListDeclaration() const = 0;

  /// isValidGCCRegisterName - Returns whether the passed in string
  /// is a valid register name according to GCC. This is used by Sema for
  /// inline asm statements.
  bool isValidGCCRegisterName(const char *Name) const;

  // getNormalizedGCCRegisterName - Returns the "normalized" GCC register name.
  // For example, on x86 it will return "ax" when "eax" is passed in.
  const char *getNormalizedGCCRegisterName(const char *Name) const;
  
  enum ConstraintInfo {
    CI_None = 0x00,
    CI_AllowsMemory = 0x01,
    CI_AllowsRegister = 0x02,
    CI_ReadWrite = 0x04
  };

  // validateOutputConstraint, validateInputConstraint - Checks that
  // a constraint is valid and provides information about it.
  // FIXME: These should return a real error instead of just true/false.
  bool validateOutputConstraint(const char *Name, ConstraintInfo &Info) const;
  bool validateInputConstraint(const char *Name, 
                               const std::string *OutputNamesBegin,
                               const std::string *OutputNamesEnd,
                               ConstraintInfo* OutputConstraints,
                               ConstraintInfo &info) const;
  bool resolveSymbolicName(const char *&Name,
                           const std::string *OutputNamesBegin,
                           const std::string *OutputNamesEnd,
                           unsigned &Index) const;
  
  virtual std::string convertConstraint(const char Constraint) const {
    return std::string(1, Constraint);
  }
  
  // Returns a string of target-specific clobbers, in LLVM format.
  virtual const char *getClobbers() const = 0;
  

  /// getTargetPrefix - Return the target prefix used for identifying
  /// llvm intrinsics.
  virtual const char *getTargetPrefix() const = 0;
    
  /// getTargetTriple - Return the target triple of the primary target.
  const char *getTargetTriple() const {
    return Triple.c_str();
  }
  
  /// getDefaultFeatures - Get the default set of target features for
  /// the \args CPU; this should include all legal feature strings on
  /// the target.
  virtual void getDefaultFeatures(const std::string &CPU, 
                                  llvm::StringMap<bool> &Features) const {
  }

  /// setFeatureEnabled - Enable or disable a specific target feature,
  /// the feature name must be valid.
  ///
  /// \return - False on error (invalid feature name).
  virtual bool setFeatureEnabled(llvm::StringMap<bool> &Features,
                                 const std::string &Name,
                                 bool Enabled) const {
    return false;
  }

  void getTargetDescription(std::string& str);

  struct GCCRegAlias {
    const char * const Aliases[5];
    const char * const Register;
  };

  virtual bool useGlobalsForAutomaticVariables() const { return false; }

  /// getDefaultLangOptions - Allow the target to specify default settings for
  /// various language options.  These may be overridden by command line
  /// options. 
  virtual void getDefaultLangOptions(LangOptions &Opts) {}

  /// HandleTargetOptions - Perform initialization based on the user
  /// configured set of features.
  virtual void HandleTargetFeatures(const llvm::StringMap<bool> &Features) {
  }
protected:
  virtual uint64_t getPointerWidthV(unsigned AddrSpace) const {
    return PointerWidth;
  }
  virtual uint64_t getPointerAlignV(unsigned AddrSpace) const {
    return PointerAlign;
  }
  virtual enum IntType getPtrDiffTypeV(unsigned AddrSpace) const {
    return PtrDiffType;
  }
  virtual void getGCCRegNames(const char * const *&Names, 
                              unsigned &NumNames) const = 0;
  virtual void getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                unsigned &NumAliases) const = 0;
public:
  virtual bool validateAsmConstraint(const char *&Name, 
                                     TargetInfo::ConstraintInfo &info) const= 0;
};

}  // end namespace ellcc

#endif