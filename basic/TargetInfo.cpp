//===--- TargetInfo.cpp - Information about Target machine ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements the TargetInfo and TargetInfoImpl interfaces.
//
//===----------------------------------------------------------------------===//

#include "TargetInfo.h"
#include "llvm/LLVMContext.h"
#include "llvm/DerivedTypes.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include <cstdlib>
#include <sstream>
using namespace llvm;
using namespace ellcc;

// TargetInfo Constructor.
TargetInfo::TargetInfo(const std::string &T) : Triple(T) {
  // Set defaults.  Defaults are set for a 32-bit RISC platform,
  // like PPC or SPARC.
  // These should be overridden by concrete targets as needed.
  BigEndian = false;
  CharIsSigned = true;
  CharWidth(8); CharAlign(8);
  CharPrefAlign(0);
  WCharWidth(32); WCharAlign(32);
  WCharPrefAlign(0);
  BoolWidth(1); BoolAlign(8);
  BoolPrefAlign(0);
  ShortWidth(16); ShortAlign(16);
  ShortPrefAlign(0);
  IntWidth(32); IntAlign(32);
  IntPrefAlign(0);
  LongWidth(32); LongAlign(32);
  LongPrefAlign(0);
  LongLongWidth(64); LongLongAlign(64);
  LongLongPrefAlign(0);
  FloatWidth(32); FloatAlign(32);
  FloatPrefAlign(0);
  FloatFormat = &APFloat::IEEEsingle;

  DoubleWidth(64); DoubleAlign(64);
  DoublePrefAlign(0);
  DoubleFormat = &APFloat::IEEEdouble;

  LongDoubleWidth(128); LongDoubleAlign(128);
  LongDoublePrefAlign(0);
  LongDoubleFormat = &APFloat::IEEEquad;

  FloatImaginaryWidth(FloatWidth());
  FloatImaginaryAlign(FloatAlign());
  FloatImaginaryPrefAlign(FloatPrefAlign());
  DoubleImaginaryWidth(DoubleWidth());
  DoubleImaginaryAlign(DoubleAlign());
  DoubleImaginaryPrefAlign(DoublePrefAlign());
  LongDoubleImaginaryWidth(LongDoubleWidth());
  LongDoubleImaginaryAlign(LongDoubleAlign());
  LongDoubleImaginaryPrefAlign(LongDoublePrefAlign());

  PointerWidth(32); PointerAlign(32);
  PointerPrefAlign(0);
  VectorWidth(64); VectorAlign(64);
  VectorPrefAlign(64);
  LongVectorWidth(128); LongVectorAlign(128);
  LongVectorPrefAlign(0);

  AggregateWidth(0);
  AggregateAlign(0);
  AggregatePrefAlign(0);

  IntMaxTWidth = 64;
  SizeType = UnsignedLong;
  PtrDiffType = Long;
  IntMaxType = LongLong;
  UIntMaxType = UnsignedLongLong;
  IntPtrType = Int;
  UIntPtrType = UnsignedInt;
  WCharType = Int;

  UserLabelPrefix = "_";
}

// Out of line virtual dtor for TargetInfo.
TargetInfo::~TargetInfo() {}

// Get the target description string, e.g.:
// "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64"

#define xstr(x) #x
#define str(x) xstr(x)
#define DATA(type, name)                            \
    str << "-" str(type);                           \
    str << (int)typeInfo[name].Width;               \
    str << ":";                                     \
    str << (int)typeInfo[name].Align;               \
    if (typeInfo[name].Align) {                     \
        str << ":";                                 \
        str << (int)typeInfo[name].PrefAlign;       \
    }

#define DATAP(type, name)                           \
    str << "-" str(type);                           \
    str << ":";                                     \
    str << (int)typeInfo[name].Width;               \
    str << ":";                                     \
    str << (int)typeInfo[name].Align;               \
    if (typeInfo[name].PrefAlign) {                 \
        str << ":";                                 \
        str << (int)typeInfo[name].PrefAlign;       \
    }

#define DATAN(type, name)                           \
    str << "-" str(type);                           \
    str << ":";                                     \
    str << (int)typeInfo[name].Align;               \
    if (typeInfo[name].PrefAlign) {                 \
        str << ":";                                 \
        str << (int)typeInfo[name].PrefAlign;       \
    }

void TargetInfo::getTargetDescription(std::string& res)
{
    std::stringstream str;
    if (BigEndian) {
        str << "E";
    } else {
        str << "e";
    }
    DATAP(p, Pointer);
    DATA(i, Bool);
    DATA(i, Char);
    DATA(i, Short);
    DATA(i, Int);
    DATA(i, Long);
    DATA(i, LongLong);
    DATA(f, Float);
    DATA(f, Double);
    DATA(f, LongDouble);
    DATA(v, Vector);
    DATA(v, LongVector);
    DATAN(a0, Aggregate);
    str >> res;
}
#undef xstr
#undef str
#undef DATA
#undef DATAP
#undef DATAN

// The names of the simple types.
const char*  TargetInfo::typeNames[TypeIDCount] =
{
  "<no type>",
  "bool",
  "char",
  "signed char",
  "unsigned char",
  "wchar_t",
  "short",
  "unsigned short",
  "int",
  "unsigned int",
  "long",
  "unsigned long",
  "long long",
  "unsigned long long",
  "float",
  "double",
  "long double",
  "float _Complex",
  "double _Complex",
  "long double _Complex",
  "float _Imaginary",
  "double _Imaginary",
  "long double _Imaginary",
  "vector",
  "long vector",
  "<pointer>",
  "<aggregate>",
  "void"
};

//===----------------------------------------------------------------------===//

void TargetInfo::removeGCCRegisterPrefix(const char *&Name) const {
  if (Name[0] == getGCCRegPrefix())
    Name++;
}

/// isValidGCCRegisterName - Returns whether the passed in string
/// is a valid register name according to GCC. This is used by Sema for
/// inline asm statements.
bool TargetInfo::isValidGCCRegisterName(const char *Name) const {
  const char * const *Names;
  unsigned NumNames;
  
  // Get rid of any register prefix.
  removeGCCRegisterPrefix(Name);
  
  if (strcmp(Name, "memory") == 0 ||
      strcmp(Name, "cc") == 0)
    return true;
  
  getGCCRegNames(Names, NumNames);
  
  // If we have a number it maps to an entry in the register name array.
  if (isdigit(Name[0])) {
    char *End;
    int n = (int)strtol(Name, &End, 0);
    if (*End == 0)
      return n >= 0 && (unsigned)n < NumNames;
  }

  // Check register names.
  for (unsigned i = 0; i < NumNames; i++) {
    if (strcmp(Name, Names[i]) == 0)
      return true;
  }
  
  // Now check aliases.
  const GCCRegAlias *Aliases;
  unsigned NumAliases;
  
  getGCCRegAliases(Aliases, NumAliases);
  for (unsigned i = 0; i < NumAliases; i++) {
    for (unsigned j = 0 ; j < array_lengthof(Aliases[i].Aliases); j++) {
      if (!Aliases[i].Aliases[j])
        break;
      if (strcmp(Aliases[i].Aliases[j], Name) == 0)
        return true;
    }
  }
  
  return false;
}

const char *TargetInfo::getNormalizedGCCRegisterName(const char *Name) const {
  assert(isValidGCCRegisterName(Name) && "Invalid register passed in");
  
  removeGCCRegisterPrefix(Name);
    
  const char * const *Names;
  unsigned NumNames;

  getGCCRegNames(Names, NumNames);

  // First, check if we have a number.
  if (isdigit(Name[0])) {
    char *End;
    int n = (int)strtol(Name, &End, 0);
    if (*End == 0) {
      assert(n >= 0 && (unsigned)n < NumNames && 
             "Out of bounds register number!");
      return Names[n];
    }
  }
  
  // Now check aliases.
  const GCCRegAlias *Aliases;
  unsigned NumAliases;
  
  getGCCRegAliases(Aliases, NumAliases);
  for (unsigned i = 0; i < NumAliases; i++) {
    for (unsigned j = 0 ; j < array_lengthof(Aliases[i].Aliases); j++) {
      if (!Aliases[i].Aliases[j])
        break;
      if (strcmp(Aliases[i].Aliases[j], Name) == 0)
        return Aliases[i].Register;
    }
  }
  
  return Name;
}

bool TargetInfo::validateOutputConstraint(const char *Name, 
                                          ConstraintInfo &info) const
{
  info = CI_None;

  // An output constraint must start with '=' or '+'
  if (*Name != '=' && *Name != '+')
    return false;

  if (*Name == '+')
    info = CI_ReadWrite;
  else
    info = CI_None;

  Name++;
  while (*Name) {
    switch (*Name) {
    default:
      if (!validateAsmConstraint(Name, info)) {
        // FIXME: We temporarily return false
        // so we can add more constraints as we hit it.
        // Eventually, an unknown constraint should just be treated as 'g'.
        return false;
      }
    case '&': // early clobber.
      break;
    case 'r': // general register.
      info = (ConstraintInfo)(info|CI_AllowsRegister);
      break;
    case 'm': // memory operand.
      info = (ConstraintInfo)(info|CI_AllowsMemory);
      break;
    case 'g': // general register, memory operand or immediate integer.
    case 'X': // any operand.
      info = (ConstraintInfo)(info|CI_AllowsMemory|CI_AllowsRegister);
      break;
    }
    
    Name++;
  }
  
  return true;
}

bool TargetInfo::resolveSymbolicName(const char *&Name,
                                     const std::string *OutputNamesBegin,
                                     const std::string *OutputNamesEnd,
                                     unsigned &Index) const
{
  assert(*Name == '[' && "Symbolic name did not start with '['");

  Name++;
  const char *Start = Name;
  while (*Name && *Name != ']')
    Name++;
  
  if (!*Name) {
    // Missing ']'
    return false;
  }
  
  std::string SymbolicName(Start, Name - Start);
  
  Index = 0;
  for (const std::string *it = OutputNamesBegin; 
       it != OutputNamesEnd; 
       ++it, Index++) {
    if (SymbolicName == *it)
      return true;
  }

  return false;
}

bool TargetInfo::validateInputConstraint(const char *Name,
                                         const std::string *OutputNamesBegin,
                                         const std::string *OutputNamesEnd,
                                         ConstraintInfo* OutputConstraints,
                                         ConstraintInfo &info) const {
  info = CI_None;

  while (*Name) {
    switch (*Name) {
    default:
      // Check if we have a matching constraint
      if (*Name >= '0' && *Name <= '9') {
        unsigned NumOutputs = OutputNamesEnd - OutputNamesBegin;
        unsigned i = *Name - '0';
  
        // Check if matching constraint is out of bounds.
        if (i >= NumOutputs)
          return false;
        
        // The constraint should have the same info as the respective 
        // output constraint.
        info = (ConstraintInfo)(info|OutputConstraints[i]);
      } else if (!validateAsmConstraint(Name, info)) {
        // FIXME: This error return is in place temporarily so we can
        // add more constraints as we hit it.  Eventually, an unknown
        // constraint should just be treated as 'g'.
        return false;
      }
      break;
    case '[': {
      unsigned Index = 0;
      if (!resolveSymbolicName(Name, OutputNamesBegin, OutputNamesEnd, Index))
        return false;
    
      break;
    }          
    case '%': // commutative
      // FIXME: Fail if % is used with the last operand.
      break;
    case 'i': // immediate integer.
    case 'I':
    case 'n': // immediate integer with a known value.
      break;
    case 'r': // general register.
      info = (ConstraintInfo)(info|CI_AllowsRegister);
      break;
    case 'm': // memory operand.
      info = (ConstraintInfo)(info|CI_AllowsMemory);
      break;
    case 'g': // general register, memory operand or immediate integer.
    case 'X': // any operand.
      info = (ConstraintInfo)(info|CI_AllowsMemory|CI_AllowsRegister);
      break;
    }
    
    Name++;
  }
  
  return true;
}

void TargetInfo::getRaiseInstructionsList(LLVMContext& C, RaiseInstructionsList*& List, 
                                          unsigned &NumRaises) const
{
    getRaiseInstructionsList(List, NumRaises);
    for (unsigned index = 0; index < NumRaises; ++index) {
        const Type* RT = NULL;
        std::vector<const Type*>args;
        for (unsigned type = 0; type < List[index].Args + 1; ++type) {
            const Type* Ty;
            if (List[index].NumBits[type]) {
                Ty = IntegerType::get(C, List[index].NumBits[type]);
            } else {
                Ty = Type::getPrimitiveType(C, List[index].TypeID[type]);
            }
            if (type == 0) {
                // The return type.
                RT = Ty;
            } else {
                args.push_back(Ty);
            }
        }
        List[index].FuncType = FunctionType::get(RT, args, false);
    }
}

