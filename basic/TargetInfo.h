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

#ifndef TARGET_INFO_H
#define TARGET_INFO_H

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#define __STDC_CONSTANT_MACROS 1
#endif
#include "llvm/Type.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/ADT/StringMap.h"
#include <vector>
#include <string>

namespace llvm {
struct fltSemantics;
class FunctionType;
class LLVMContext;
}

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
  // Warning! The order of these enum values is well known in the typeInfo and
  // typeNames arrays.
  enum TypeID {
    NoType = 0,
    Bool,
    Char,
    SignedChar,
    UnsignedChar,
    WChar,
    Short,
    UnsignedShort,
    Int,
    UnsignedInt,
    Long,
    UnsignedLong,
    LongLong,
    UnsignedLongLong,
    Float,
    Double,
    LongDouble,
    FloatComplex,
    DoubleComplex,
    LongDoubleComplex,
    FloatImaginary,
    DoubleImaginary,
    LongDoubleImaginary,
    Vector,
    LongVector,
    Pointer,
    Aggregate,
    Void,
    TypeIDCount
  };

private:
  /** Info about each simple type.
   */
  struct TypeInfo {
    unsigned char Width;        ///< The type width in bytes.
    unsigned char Align;        ///< The type's ABI alignment requirement.
    unsigned char PrefAlign;    ///< The type's preferred alignment.
  };
 TypeInfo typeInfo[TypeIDCount];
 static const char* typeNames[TypeIDCount];

protected:
  TypeID SizeType, IntMaxType, UIntMaxType, PtrDiffType, IntPtrType, UIntPtrType, WCharType;
public:
  // Define the simple type access functions.
  // e.g.:
  // void CharWidth(unsigned char Width)      { typeInfo[Char].Width = Width; }
  // unsigned char CharWidth()                { return typeInfo[Char].Width; }
#define TYPE_FUNC(type, value) \
  void type##value(unsigned char value) { typeInfo[type].value = value; } \
  unsigned char type##value() const { return typeInfo[type].value; }

  // Define all three access functions for a type.
#define TYPE_FUNCS(type) \
  TYPE_FUNC(type, Width) \
  TYPE_FUNC(type, Align) \
  TYPE_FUNC(type, PrefAlign)
  
#define TYPE_FUNCU(type, value) \
  void type##value(unsigned char value) { typeInfo[type].value = value;             \
                                          typeInfo[Unsigned##type].value = value; } \
  unsigned char type##value() const { return typeInfo[type].value; }

  // Define all three access functions for a type.
#define TYPE_FUNCSU(type) \
  TYPE_FUNCU(type, Width) \
  TYPE_FUNCU(type, Align) \
  TYPE_FUNCU(type, PrefAlign)

  // Treat the three types of char as "Char".
  void CharWidth(unsigned char Width)                   { typeInfo[Char].Width = Width;
                                                          typeInfo[SignedChar].Width = Width;
                                                          typeInfo[UnsignedChar].Width = Width; }
  unsigned char CharWidth() const                       { return typeInfo[Char].Width; }
  void CharAlign(unsigned char Align)                   { typeInfo[Char].Align = Align;
                                                          typeInfo[SignedChar].Align = Align;
                                                          typeInfo[UnsignedChar].Align = Align; }
  unsigned char CharAlign() const                       { return typeInfo[Char].Align; }
  void CharPrefAlign(unsigned char PrefAlign)               { typeInfo[Char].PrefAlign = PrefAlign;
                                                          typeInfo[SignedChar].PrefAlign = PrefAlign;
                                                          typeInfo[UnsignedChar].PrefAlign = PrefAlign; }
  unsigned char CharPrefAlign() const                   { return typeInfo[Char].PrefAlign; }
  TYPE_FUNCS(WChar)
  TYPE_FUNCS(Bool)
  TYPE_FUNCSU(Short)
  TYPE_FUNCSU(Int)
  TYPE_FUNCSU(Long)
  TYPE_FUNCSU(LongLong)
  TYPE_FUNCS(Float)
  TYPE_FUNCS(Double)
  TYPE_FUNCS(LongDouble)
  TYPE_FUNCS(Pointer)
  TYPE_FUNCS(Vector)
  TYPE_FUNCS(LongVector)
  TYPE_FUNCS(Aggregate)
  TYPE_FUNCS(FloatImaginary)
  TYPE_FUNCS(DoubleImaginary)
  TYPE_FUNCS(LongDoubleImaginary)
#undef TYPE_FUNC
#undef TYPE_FUNCS
#undef TYPE_FUNCU
#undef TYPE_FUNCSU

  TypeID getSizeType() const { return SizeType; }
  TypeID getIntMaxType() const { return IntMaxType; }
  TypeID getUIntMaxType() const { return UIntMaxType; }
  TypeID getPtrDiffType(unsigned AddrSpace) const {
    return AddrSpace == 0 ? PtrDiffType : getPtrDiffTypeV(AddrSpace);
  }
  TypeID getIntPtrType() const { return IntPtrType; }
  TypeID getUIntPtrType() const { return UIntPtrType; }
  TypeID getWCharType() const { return WCharType; }
  unsigned char getTypeSizeInBits(TypeID id) { return typeInfo[id].Width; }
  unsigned char getTypeSizeInBytes(TypeID id)
    { unsigned size = typeInfo[id].Width / CharWidth(); return size ? size : 1; }
  unsigned char getTypeAlignInBits(TypeID id) { return typeInfo[id].Align; }
  unsigned char getTypeAlignInBytes(TypeID id)
    { unsigned align = typeInfo[id].Align / CharWidth(); return align ? align : 1; }


  /// isCharSigned - Return true if 'char' is 'signed char' or false if it is
  /// treated as 'unsigned char'.  This is implementation defined according to
  /// C99 6.2.5p15.  In our implementation, this is target-specific.
  bool isCharSigned() const { return CharIsSigned; }
  
  /// getCharWidth - Return the width a char or wide char.
  uint64_t getCharWidth(bool isWide) const {
    return isWide ? typeInfo[WChar].Width : typeInfo[Char].Width;
  }

  /// getPointerWidth - Return the width of pointers on this target, for the
  /// specified address space.
  uint64_t getPointerWidth(unsigned AddrSpace) const {
    return AddrSpace == 0 ? typeInfo[Pointer].Width : getPointerWidthV(AddrSpace);
  }
  uint64_t getPointerAlign(unsigned AddrSpace) const {
    return AddrSpace == 0 ? typeInfo[Pointer].Align : getPointerAlignV(AddrSpace);
  }
  
  /// getFloatFormat - Return the format of 'float'.
  const llvm::fltSemantics &getFloatFormat() const { return *FloatFormat; }

  /// getDoubleFormat - Return the format of 'double'.
  const llvm::fltSemantics &getDoubleFormat() const { return *DoubleFormat; }

  /// getLongDoubleFormat - Return the format of 'long double'.
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
  static const char *getTypeName(TypeID T)
    { return typeNames[T]; }
  
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
    CI_None             = 0x00,
    CI_AllowsMemory     = 0x01,
    CI_AllowsRegister   = 0x02,
    CI_ReadWrite        = 0x04,
    CI_IsRegister       = 0x08
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

  /** A list of instructions to raise to function calls.
   */
  static const unsigned NumRaiseTypes = 5;
  struct RaiseInstructionsList {
    short Instruction;                                  // The instruction to match.
    const char* Name;                                   // The function that replaces the instruction.
    unsigned Args;                                      // The number of arguments.
    const llvm::Type::TypeID TypeID[NumRaiseTypes];     // The TypeIDs of the return value and arguments.
    const unsigned NumBits[NumRaiseTypes];              // != 0 if an integer type.
    llvm::FunctionType* FuncType;                       // The calculated function type.
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
    return typeInfo[Pointer].Width;
  }
  virtual uint64_t getPointerAlignV(unsigned AddrSpace) const {
    return typeInfo[Pointer].Align;
  }
  virtual enum TypeID getPtrDiffTypeV(unsigned AddrSpace) const {
    return PtrDiffType;
  }
  virtual void getGCCRegNames(const char * const *&Names, 
                              unsigned &NumNames) const = 0;
  virtual void getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                unsigned &NumAliases) const = 0;
  void removeGCCRegisterPrefix(const char *&Name) const;
  virtual void getRaiseInstructionsList(RaiseInstructionsList*& List, 
                                unsigned &NumRaises) const {
    List = NULL;
    NumRaises = 0;
  }

public:
  void getRaiseInstructionsList(llvm::LLVMContext& C, RaiseInstructionsList*& List, 
                                unsigned &NumRaises) const;
  virtual char getGCCRegPrefix() const { return '%'; };
  virtual bool validateAsmConstraint(const char *&Name, 
                                     TargetInfo::ConstraintInfo &info) const= 0;
};

}  // end namespace ellcc

#endif
