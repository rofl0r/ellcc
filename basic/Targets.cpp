//===--- Targets.cpp - Implement -arch option and targets -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements construction of a TargetInfo object from a 
// target triple.
//
//===----------------------------------------------------------------------===//

#include "Builtins.h"
#include "TargetBuiltins.h"
#include "TargetInfo.h"
#include "LangOptions.h"
#include "llvm/Instruction.h"
#include "llvm/Type.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/SmallString.h"
using namespace llvm;
using namespace ellcc;

//===----------------------------------------------------------------------===//
//  Common code shared among targets.
//===----------------------------------------------------------------------===//

static void Define(std::vector<char> &Buf, const llvm::StringRef &Macro,
                   const llvm::StringRef &Val = "1") {
  const char *Def = "#define ";
  Buf.insert(Buf.end(), Def, Def+strlen(Def));
  Buf.insert(Buf.end(), Macro.begin(), Macro.end());
  Buf.push_back(' ');
  Buf.insert(Buf.end(), Val.begin(), Val.end());
  Buf.push_back('\n');
}

/// DefineStd - Define a macro name and standard variants.  For example if
/// MacroName is "unix", then this will define "__unix", "__unix__", and "unix"
/// when in GNU mode.
static void DefineStd(std::vector<char> &Buf, const char *MacroName, 
                      const LangOptions &Opts) {
  assert(MacroName[0] != '_' && "Identifier should be in the user's namespace");
  
  // If in GNU mode (e.g. -std=gnu99 but not -std=c99) define the raw identifier
  // in the user's namespace.
  if (Opts.GNUMode)
    Define(Buf, MacroName);
  
  // Define __unix.
  SmallString<20> TmpStr;
  TmpStr = "__";
  TmpStr += MacroName;
  Define(Buf, TmpStr.str());
  
  // Define __unix__.
  TmpStr += "__";
  Define(Buf, TmpStr.str());
}

//===----------------------------------------------------------------------===//
// Defines specific to certain operating systems.
//===----------------------------------------------------------------------===//

static void getSolarisDefines(std::vector<char> &Defines) {
  Define(Defines, "__SUN__");
  Define(Defines, "__SOLARIS__");
}

static void getFreeBSDDefines(const LangOptions &Opts, bool is64Bit,
                              const char *Triple, std::vector<char> &Defines) {
  // FreeBSD defines; list based off of gcc output

  const char *FreeBSD = strstr(Triple, "-freebsd");
  FreeBSD += strlen("-freebsd");
  char release[] = "X";
  release[0] = FreeBSD[0];
  char version[] = "X00001";
  version[0] = FreeBSD[0];

  Define(Defines, "__FreeBSD__", release);
  Define(Defines, "__FreeBSD_cc_version", version);
  Define(Defines, "__KPRINTF_ATTRIBUTE__");
  DefineStd(Defines, "unix", Opts);
  Define(Defines, "__ELF__", "1");
  if (is64Bit) {
    Define(Defines, "__LP64__");
  }
}

static void getDragonFlyDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) {
  // DragonFly defines; list based off of gcc output
  Define(Defines, "__DragonFly__");
  Define(Defines, "__DragonFly_cc_version", "100001");
  Define(Defines, "__ELF__");
  Define(Defines, "__KPRINTF_ATTRIBUTE__");
  Define(Defines, "__tune_i386__");
  DefineStd(Defines, "unix", Opts);
}

static void getLinuxDefines(const LangOptions &Opts, std::vector<char> &Defines) {
  // Linux defines; list based off of gcc output
  DefineStd(Defines, "unix", Opts);
  DefineStd(Defines, "linux", Opts);
  Define(Defines, "__gnu_linux__");
  Define(Defines, "__ELF__", "1");
}

/// getDarwinNumber - Parse the 'darwin number' out of the specific targe
/// triple.  For example, if we have darwin8.5 return 8,5,4.  If any entry is
/// not defined, return 0's.  Return true if we have -darwin in the string or
/// false otherwise.
static bool getDarwinNumber(const char *Triple, unsigned &Maj, unsigned &Min) {
  Maj = Min = 0;
  const char *Darwin = strstr(Triple, "-darwin");
  if (Darwin == 0) return false;
  
  Darwin += strlen("-darwin");
  if (Darwin[0] < '0' || Darwin[0] > '9')
    return true;
  
  Maj = Darwin[0]-'0';
  ++Darwin;
    
  // Handle "darwin11".
  if (Maj == 1 && Darwin[0] >= '0' && Darwin[0] <= '9') {
    Maj = 10+Darwin[0]-'0';
    ++Darwin;
  }
    
  // Handle minor version: 10.4.9 -> darwin8.9 -> "1049"
  if (Darwin[0] == '.' && Darwin[1] >= '0' && Darwin[1] <= '9' &&
      Darwin[2] == '\0')
    Min = Darwin[1]-'0';
  
  return true;
}

static void getDarwinDefines(std::vector<char> &Defines, const char *Triple) {
  Define(Defines, "__APPLE__");
  Define(Defines, "__MACH__");
  
  // Figure out which "darwin number" the target triple is.  "darwin9" -> 10.5.
  unsigned Maj, Min;
  if (getDarwinNumber(Triple, Maj, Min)) {
    char DarwinStr[] = "1000";
    if (Maj >= 4 && Maj <= 13) { // 10.0-10.9
      // darwin7 -> 1030, darwin8 -> 1040, darwin9 -> 1050, etc.
      DarwinStr[2] = '0' + Maj-4;
    }
      
    // Handle minor version: 10.4.9 -> darwin8.9 -> "1049"
    DarwinStr[3] = Min+'0';
    Define(Defines, "__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__", DarwinStr);
  }
}

/// GetDarwinLanguageOptions - Set the default language options for darwin.
static void GetDarwinLanguageOptions(LangOptions &Opts,
                                     const char *Triple) {
  unsigned Maj, Min;
  if (!getDarwinNumber(Triple, Maj, Min))
    return;
  
  // Blocks default to on for 10.6 (darwin10) and beyond.
  // As does nonfragile-abi for 64bit mode
  if (Maj > 9) 
    Opts.Blocks = 1;
}


//===----------------------------------------------------------------------===//
// Specific target implementations.
//===----------------------------------------------------------------------===//

namespace {
// Alpha target abstract base class.
class AlphaTargetInfo : public TargetInfo {
  static const Builtin::Info BuiltinInfo[];
  static const char * const GCCRegNames[];
public:
  AlphaTargetInfo(const std::string& triple) 
    : TargetInfo(triple) {
  }
  virtual void getTargetBuiltins(const Builtin::Info *&Records,
                                 unsigned &NumRecords) const {
    Records = BuiltinInfo;
    NumRecords = ellcc::Alpha::LastTSBuiltin-Builtin::FirstTSBuiltin;
  }
  virtual const char *getClobbers() const {return "";}
  virtual const char *getTargetPrefix() const {
    return "alpha";
  }  
  virtual void getGCCRegNames(const char * const *&Names, 
                              unsigned &NumNames) const;
  virtual bool validateAsmConstraint(const char *&Name,
                                     TargetInfo::ConstraintInfo &info) const;
  virtual std::string convertConstraint(const char Constraint) const;
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const;
  virtual const char *getVAListDeclaration() const {
    return "typedef char* __builtin_va_list;";
  }
  virtual void getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                unsigned &NumAliases) const {
    // FIXME: Implement.
    Aliases = 0;
    NumAliases = 0;
  }
};

const Builtin::Info AlphaTargetInfo::BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS, ...) { #ID, TYPE, ATTRS, 0, false, { __VA_ARGS__ } },
#define LIBBUILTIN(ID, TYPE, ATTRS, HEADER, ...) { #ID, TYPE, ATTRS, HEADER, false, { __VA_ARGS__ } },
#include "AlphaBuiltins.def"
};
  
const char * const AlphaTargetInfo::GCCRegNames[] = {
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
  "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
  "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",
  "r32", "r33", "r34", "r35", "r36", "r37", "r38", "r39",
  "r40", "r41", "r42", "r43", "r44", "r45", "r46", "r47",
  "r48", "r49", "r50", "r51", "r52", "r53", "r54", "r55",
  "r56", "r57", "r58", "r59", "r60", "r61", "r62", "r63"
};

void AlphaTargetInfo::getGCCRegNames(const char * const *&Names, 
                                     unsigned &NumNames) const
{
  Names = GCCRegNames;
  NumNames = array_lengthof(GCCRegNames);
}

/// AlphaTargetInfo::getTargetDefines - Return a set of the Alpha-specific #defines
/// that are not tied to a specific subtarget.
void AlphaTargetInfo::getTargetDefines(const LangOptions &Opts,
                                       std::vector<char> &Defines) const {
  // Target identification.
  DefineStd(Defines, "__alpha__", Opts);
  DefineStd(Defines, "alpha", Opts);
  
  // Target properties. FIXME: Big or little.
  Define(Defines, "__LITTLE_ENDIAN__");
  
  // Subtarget options.
  Define(Defines, "__REGISTER_PREFIX__", "");
}
  
// FIXME
bool
AlphaTargetInfo::validateAsmConstraint(const char *&Name,
                                       TargetInfo::ConstraintInfo &info) const
{
    switch (*Name)
    {
    default: return false;
    }
}

// FIXME
std::string
AlphaTargetInfo::convertConstraint(const char Constraint) const {
  switch (Constraint) {
  case 'a': return std::string("{r24}");
  case 'b': return std::string("{r25}");
  case 'c': return std::string("{r27}");
  case 'v': return std::string("{r0}");
  default:
    return std::string(1, Constraint);
  }
}
} // end anonymous namespace

namespace {
// PPC abstract base class
class PPCTargetInfo : public TargetInfo {
  static const Builtin::Info BuiltinInfo[];
  static const char * const GCCRegNames[];
  static const TargetInfo::GCCRegAlias GCCRegAliases[];

public:
  PPCTargetInfo(const std::string& triple) : TargetInfo(triple) {
    CharIsSigned = false;
  }
  virtual void getTargetBuiltins(const Builtin::Info *&Records,
                                 unsigned &NumRecords) const {
    Records = BuiltinInfo;
    NumRecords = ellcc::PPC::LastTSBuiltin-Builtin::FirstTSBuiltin;
  }
  
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const;
  
  virtual const char *getVAListDeclaration() const {
    return "typedef char* __builtin_va_list;";
    // This is the right definition for ABI/V4: System V.4/eabi.
    /*return "typedef struct __va_list_tag {"
           "  unsigned char gpr;"
           "  unsigned char fpr;"
           "  unsigned short reserved;"
           "  void* overflow_arg_area;"
           "  void* reg_save_area;"
           "} __builtin_va_list[1];";*/
  }
  virtual const char *getTargetPrefix() const {
    return "ppc";
  }
  virtual void getGCCRegNames(const char * const *&Names, 
                              unsigned &NumNames) const;
  virtual void getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                unsigned &NumAliases) const;
  virtual bool validateAsmConstraint(const char *&Name,
                                     TargetInfo::ConstraintInfo &info) const;
  virtual const char *getClobbers() const {
    return "";
  }
};

const Builtin::Info PPCTargetInfo::BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS, ...) { #ID, TYPE, ATTRS, 0, false, { __VA_ARGS__ } },
#define LIBBUILTIN(ID, TYPE, ATTRS, HEADER, ...) { #ID, TYPE, ATTRS, HEADER, false, { __VA_ARGS__ } },
#include "PPCBuiltins.def"
};
  
  
/// PPCTargetInfo::getTargetDefines - Return a set of the PowerPC-specific
/// #defines that are not tied to a specific subtarget.
void PPCTargetInfo::getTargetDefines(const LangOptions &Opts,
                                     std::vector<char> &Defines) const {
  // Target identification.
  Define(Defines, "_ARCH_PPC");
  Define(Defines, "__POWERPC__");
  if (PointerWidth() == 64) {
    Define(Defines, "__powerpc64__");
    Define(Defines, "_ARCH_PPC64");
    Define(Defines, "_LP64");
    Define(Defines, "__LP64__");
    Define(Defines, "__ppc64__");
  } else {
    Define(Defines, "__powerpc__");
    Define(Defines, "__ppc__");
  }
  
  // Target properties.
  Define(Defines, "_BIG_ENDIAN");
  Define(Defines, "__BIG_ENDIAN__");
  
  // Subtarget options.
  Define(Defines, "__NATURAL_ALIGNMENT__");
  Define(Defines, "__REGISTER_PREFIX__", "");
  
  // FIXME: Should be controlled by command line option.
  Define(Defines, "__LONG_DOUBLE_128__");
}

const char * const PPCTargetInfo::GCCRegNames[] = {
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
  "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
  "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",
  "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
  "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15",
  "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
  "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31",
  "mq", "lr", "ctr", "ap",
  "cr0", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7",
  "xer",
  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15",
  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31",
  "vrsave", "vscr",
  "spe_acc", "spefscr",
  "sfp"
};

void PPCTargetInfo::getGCCRegNames(const char * const *&Names, 
                                   unsigned &NumNames) const {
  Names = GCCRegNames;
  NumNames = array_lengthof(GCCRegNames);
}

const TargetInfo::GCCRegAlias PPCTargetInfo::GCCRegAliases[] = {
  // While some of these aliases do map to different registers
  // they still share the same register name.
  { { "0" }, "r0" }, 
  { { "1"}, "r1" }, 
  { { "2" }, "r2" }, 
  { { "3" }, "r3" }, 
  { { "4" }, "r4" }, 
  { { "5" }, "r5" }, 
  { { "6" }, "r6" }, 
  { { "7" }, "r7" }, 
  { { "8" }, "r8" }, 
  { { "9" }, "r9" }, 
  { { "10" }, "r10" }, 
  { { "11" }, "r11" }, 
  { { "12" }, "r12" }, 
  { { "13" }, "r13" }, 
  { { "14" }, "r14" }, 
  { { "15" }, "r15" }, 
  { { "16" }, "r16" }, 
  { { "17" }, "r17" }, 
  { { "18" }, "r18" }, 
  { { "19" }, "r19" }, 
  { { "20" }, "r20" }, 
  { { "21" }, "r21" }, 
  { { "22" }, "r22" }, 
  { { "23" }, "r23" }, 
  { { "24" }, "r24" }, 
  { { "25" }, "r25" }, 
  { { "26" }, "r26" }, 
  { { "27" }, "r27" }, 
  { { "28" }, "r28" }, 
  { { "29" }, "r29" }, 
  { { "30" }, "r30" }, 
  { { "31" }, "r31" }, 
  { { "fr0" }, "f0" }, 
  { { "fr1" }, "f1" }, 
  { { "fr2" }, "f2" }, 
  { { "fr3" }, "f3" }, 
  { { "fr4" }, "f4" }, 
  { { "fr5" }, "f5" }, 
  { { "fr6" }, "f6" }, 
  { { "fr7" }, "f7" }, 
  { { "fr8" }, "f8" }, 
  { { "fr9" }, "f9" }, 
  { { "fr10" }, "f00" }, 
  { { "fr11" }, "f11" }, 
  { { "fr12" }, "f12" }, 
  { { "fr13" }, "f13" }, 
  { { "fr14" }, "f14" }, 
  { { "fr15" }, "f15" }, 
  { { "fr16" }, "f16" }, 
  { { "fr17" }, "f17" }, 
  { { "fr18" }, "f18" }, 
  { { "fr19" }, "f19" }, 
  { { "fr20" }, "f20" }, 
  { { "fr21" }, "f21" }, 
  { { "fr22" }, "f22" }, 
  { { "fr23" }, "f23" }, 
  { { "fr24" }, "f24" }, 
  { { "fr25" }, "f25" }, 
  { { "fr26" }, "f26" }, 
  { { "fr27" }, "f27" }, 
  { { "fr28" }, "f28" }, 
  { { "fr29" }, "f29" }, 
  { { "fr30" }, "f30" }, 
  { { "fr31" }, "f31" }, 
  { { "cc" }, "cr0" }, 
};

void PPCTargetInfo::getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                     unsigned &NumAliases) const {
  Aliases = GCCRegAliases;
  NumAliases = array_lengthof(GCCRegAliases);
}

bool PPCTargetInfo::validateAsmConstraint(const char *&Name,
                                          TargetInfo::ConstraintInfo &info) const
{
    switch (*Name) {
    default: return false;
    case 'b': // Base register.
    case 'f': // Floating point register.
    case 'v': // Vector register.
    case 'h': // mq, ctr, or link register.
    case 'q': // mq register.
    case 'c': // ctr register.
    case 'l': // link register.
    case 'x': // cr register 0.
    case 'y': // cr register.
    case 'z': // fpmem register.
        info = (TargetInfo::ConstraintInfo)(info|TargetInfo::CI_AllowsRegister);
        return true;
    case 'I': // Signed 16-bit constant.
    case 'J': // Unsigned 16-bit constant.
    case 'K': // Unsigned 16-bit constant shifted left 16 bits.
    case 'L': // Signed 16-bit constant shifted left 16 bits.
    case 'M': // Constant larger than 31.
    case 'N': // An exact power of two.
    case 'O': // Zero
    case 'P': // Signed 16-bit negated constant.
    case 'G': // Floating point constant that can be loaded with one instruction.
    case 'H': // Integer or Floating point constant that can be loaded with three instructions.
    case 'S': // 64-bit mask operand.
    case 'T': // 64-bit mask operand.
    case 't': // AND masks for two rldic[lr] instructions..
    case 'W': // Non-memory vector constant..
        return true;
    }
}

} // end anonymous namespace.

namespace {
class PPC32TargetInfo : public PPCTargetInfo {
public:
  PPC32TargetInfo(const std::string& triple) : PPCTargetInfo(triple) {
    LongDoubleFormat = &APFloat::PPCDoubleDouble;
  }
};
} // end anonymous namespace.

namespace {
class PPC64TargetInfo : public PPCTargetInfo {
public:
  PPC64TargetInfo(const std::string& triple) : PPCTargetInfo(triple) {
    LongWidth(64); LongAlign(64); PointerWidth(64); PointerAlign(64);
    SizeType = UnsignedLong;
    PtrDiffType = Long;
    LongDoubleFormat = &APFloat::PPCDoubleDouble;
    IntPtrType = Long;
    UIntPtrType = UnsignedLong;
  }
};
} // end anonymous namespace.


namespace {
class DarwinPPCTargetInfo : public PPC32TargetInfo {
public:
  DarwinPPCTargetInfo(const std::string& triple) : PPC32TargetInfo(triple) {}
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    PPC32TargetInfo::getTargetDefines(Opts, Defines);
    getDarwinDefines(Defines, getTargetTriple());
  }

  /// getDefaultLangOptions - Allow the target to specify default settings for
  /// various language options.  These may be overridden by command line
  /// options. 
  virtual void getDefaultLangOptions(LangOptions &Opts) {
    GetDarwinLanguageOptions(Opts, getTargetTriple());
  }
};
} // end anonymous namespace.

namespace {
class DarwinPPC64TargetInfo : public PPC64TargetInfo {
public:
  DarwinPPC64TargetInfo(const std::string& triple) : PPC64TargetInfo(triple) {}
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    PPC64TargetInfo::getTargetDefines(Opts, Defines);
    getDarwinDefines(Defines, getTargetTriple());
  }

  /// getDefaultLangOptions - Allow the target to specify default settings for
  /// various language options.  These may be overridden by command line
  /// options. 
  virtual void getDefaultLangOptions(LangOptions &Opts) {
    GetDarwinLanguageOptions(Opts, getTargetTriple());
  }
};
} // end anonymous namespace.

namespace {
// Namespace for x86 abstract base class
const Builtin::Info BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS, ...) { #ID, TYPE, ATTRS, 0, false, { __VA_ARGS__ } },
#define LIBBUILTIN(ID, TYPE, ATTRS, HEADER, ...) { #ID, TYPE, ATTRS, HEADER, false, { __VA_ARGS__ } },
#include "X86Builtins.def"
};

const char *GCCRegNames[] = {
  "ax", "dx", "cx", "bx", "si", "di", "bp", "sp",
  "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)",
  "argp", "flags", "fspr", "dirflag", "frame",
  "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",
  "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7",
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
  "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"
};

const TargetInfo::GCCRegAlias GCCRegAliases[] = {
  { { "al", "ah", "eax", "rax" }, "ax" },
  { { "bl", "bh", "ebx", "rbx" }, "bx" },
  { { "cl", "ch", "ecx", "rcx" }, "cx" },
  { { "dl", "dh", "edx", "rdx" }, "dx" },
  { { "esi", "rsi" }, "si" },
  { { "edi", "rdi" }, "di" },
  { { "esp", "rsp" }, "sp" },
  { { "ebp", "rbp" }, "bp" },
};

// X86 target abstract base class; x86-32 and x86-64 are very close, so
// most of the implementation can be shared.
class X86TargetInfo : public TargetInfo {
  enum X86SSEEnum {
    NoMMXSSE, MMX, SSE1, SSE2, SSE3, SSSE3, SSE41, SSE42
  } SSELevel;
public:
  X86TargetInfo(const std::string& triple) 
    : TargetInfo(triple),
      // FIXME: hard coding to SSE2 for now.  This should change to NoMMXSSE so
      // that the driver controls this.
      SSELevel(SSE2) {
    LongDoubleFormat = &APFloat::x87DoubleExtended;
  }
  virtual void getTargetBuiltins(const Builtin::Info *&Records,
                                 unsigned &NumRecords) const {
    Records = BuiltinInfo;
    NumRecords = ellcc::X86::LastTSBuiltin-Builtin::FirstTSBuiltin;
  }
  virtual const char *getTargetPrefix() const {
    return "x86";
  }  
  virtual void getGCCRegNames(const char * const *&Names, 
                              unsigned &NumNames) const {
    Names = GCCRegNames;
    NumNames = array_lengthof(GCCRegNames);
  }
  virtual void getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                unsigned &NumAliases) const {
    Aliases = GCCRegAliases;
    NumAliases = array_lengthof(GCCRegAliases);
  }
  virtual bool validateAsmConstraint(const char *&Name,
                                     TargetInfo::ConstraintInfo &info) const;
  virtual std::string convertConstraint(const char Constraint) const;
  virtual const char *getClobbers() const {
    return "~{dirflag},~{fpsr},~{flags}";
  }
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const;
  
  virtual void HandleTargetFeatures(const StringMap<bool> &Features);
};

/// HandleTargetOptions - Handle target-specific options like -msse2 and
/// friends.  An array of arguments is passed in: if they are all valid, this
/// should handle them and return -1.  If there is an error, the index of the
/// invalid argument should be returned along with an optional error string.
/// HandleTargetOptions - Perform initialization based on the user
/// configured set of features.
void X86TargetInfo::HandleTargetFeatures(const StringMap<bool>&Features) {
  if (Features.lookup("sse42"))
    SSELevel = SSE42;
  else if (Features.lookup("sse41"))
    SSELevel = SSE41;
  else if (Features.lookup("ssse3"))
    SSELevel = SSSE3;
  else if (Features.lookup("sse3"))
    SSELevel = SSE3;
  else if (Features.lookup("sse2"))
    SSELevel = SSE2;
  else if (Features.lookup("sse"))
    SSELevel = SSE1;
  else if (Features.lookup("mmx"))
    SSELevel = MMX;
}


/// X86TargetInfo::getTargetDefines - Return a set of the X86-specific #defines
/// that are not tied to a specific subtarget.
void X86TargetInfo::getTargetDefines(const LangOptions &Opts,
                                     std::vector<char> &Defines) const {
  // Target identification.
  if (PointerWidth() == 64) {
    Define(Defines, "_LP64");
    Define(Defines, "__LP64__");
    Define(Defines, "__amd64__");
    Define(Defines, "__amd64");
    Define(Defines, "__x86_64");
    Define(Defines, "__x86_64__");
    Define(Defines, "__SSE3__");
  } else {
    DefineStd(Defines, "__x86__", Opts);
    DefineStd(Defines, "i386", Opts);
  }
  
  // Target properties.
  Define(Defines, "__LITTLE_ENDIAN__");
  
  // Subtarget options.
  Define(Defines, "__nocona");
  Define(Defines, "__nocona__");
  Define(Defines, "__tune_nocona__");
  Define(Defines, "__REGISTER_PREFIX__", "");

  // Each case falls through to the previous one here.
  switch (SSELevel) {
  case SSE42:
    Define(Defines, "__SSE4_2__");
  case SSE41:
    Define(Defines, "__SSE4_1__");
  case SSSE3:
    Define(Defines, "__SSSE3__");
  case SSE3:
    Define(Defines, "__SSE3__");
  case SSE2:
    Define(Defines, "__SSE2__");
    Define(Defines, "__SSE2_MATH__");  // -mfp-math=sse always implied.
  case SSE1:
    Define(Defines, "__SSE__");
    Define(Defines, "__SSE_MATH__");   // -mfp-math=sse always implied.
  case MMX:
    Define(Defines, "__MMX__");
  case NoMMXSSE:
    break;
  }
}
  
  
bool
X86TargetInfo::validateAsmConstraint(const char *&Name,
                                     TargetInfo::ConstraintInfo &info) const
{
    switch (*Name) {
    default: return false;
    case 'a': // eax.
    case 'b': // ebx.
    case 'c': // ecx.
    case 'd': // edx.
    case 'S': // esi.
    case 'D': // edi.
    case 'A': // edx:eax.
    case 't': // top of floating point stack.
    case 'u': // second from top of floating point stack.
    case 'q': // Any register accessible as [r]l: a, b, c, and d.
    case 'y': // Any MMX register.
    case 'x': // Any SSE register.
    case 'Q': // Any register accessible as [r]h: a, b, c, and d.
    case 'e': // 32-bit signed integer constant for use with zero-extending 
              // x86_64 instructions.
    case 'Z': // 32-bit unsigned integer constant for use with zero-extending 
              // x86_64 instructions.
    case 'N': // unsigned 8-bit integer constant for use with in and out
              // instructions.
        info = (TargetInfo::ConstraintInfo)(info|TargetInfo::CI_AllowsRegister);
        return true;
    }
}

std::string
X86TargetInfo::convertConstraint(const char Constraint) const {
  switch (Constraint) {
  case 'a': return std::string("{ax}");
  case 'b': return std::string("{bx}");
  case 'c': return std::string("{cx}");
  case 'd': return std::string("{dx}");
  case 'S': return std::string("{si}");
  case 'D': return std::string("{di}");
  case 't': // top of floating point stack.
    return std::string("{st}");
  case 'u': // second from top of floating point stack.
    return std::string("{st(1)}"); // second from top of floating point stack.
  default:
    return std::string(1, Constraint);
  }
}
} // end anonymous namespace

namespace {
// X86-32 generic target
class X86_32TargetInfo : public X86TargetInfo {
public:
  X86_32TargetInfo(const std::string& triple) : X86TargetInfo(triple) {
    DoubleAlign(32); LongLongAlign(32);
    LongDoubleWidth(96);
    LongDoubleAlign(32);
  }
  virtual const char *getVAListDeclaration() const {
    return "typedef char* __builtin_va_list;";
  }
  virtual void getRaiseInstructionsList(RaiseInstructionsList*& List, 
                                        unsigned &NumRaises) const;
  static RaiseInstructionsList raiseInstructionsList[];
};

TargetInfo::RaiseInstructionsList X86_32TargetInfo::raiseInstructionsList[] = {
    { Instruction::URem,        "__umoddi3",   2, { }, { 64, 64, 64 } },
    { Instruction::UDiv,        "__udivdi3",   2, { }, { 64, 64, 64 } },
};

void X86_32TargetInfo::getRaiseInstructionsList(RaiseInstructionsList*& List, 
                                                unsigned &NumRaises) const
{
  List = raiseInstructionsList;
  NumRaises = array_lengthof(raiseInstructionsList);
}

} // end anonymous namespace

namespace {
// x86-32 Darwin (OS X) target
class DarwinI386TargetInfo : public X86_32TargetInfo {
public:
  DarwinI386TargetInfo(const std::string& triple) : X86_32TargetInfo(triple) {
    LongDoubleWidth(128);
    LongDoubleAlign(128);
    PtrDiffType = Int;
  }
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    X86_32TargetInfo::getTargetDefines(Opts, Defines);
    getDarwinDefines(Defines, getTargetTriple());
  }
  /// getDefaultLangOptions - Allow the target to specify default settings for
  /// various language options.  These may be overridden by command line
  /// options. 
  virtual void getDefaultLangOptions(LangOptions &Opts) {
    GetDarwinLanguageOptions(Opts, getTargetTriple());
  }
};
} // end anonymous namespace

namespace {
// x86-32 FreeBSD target
class FreeBSDX86_32TargetInfo : public X86_32TargetInfo {
public:
  FreeBSDX86_32TargetInfo(const std::string& triple) :
      X86_32TargetInfo(triple) {
    SizeType = UnsignedInt;
    PtrDiffType = Int;
  }
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    X86_32TargetInfo::getTargetDefines(Opts, Defines);
    getFreeBSDDefines(Opts, 0, getTargetTriple(), Defines);
  }
};
} // end anonymous namespace

namespace {
// x86-32 DragonFly target
class DragonFlyX86_32TargetInfo : public X86_32TargetInfo {
public:
  DragonFlyX86_32TargetInfo(const std::string& triple) :
      X86_32TargetInfo(triple) {
    SizeType = UnsignedInt;
    PtrDiffType = Int;
  }
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    X86_32TargetInfo::getTargetDefines(Opts, Defines);
    getDragonFlyDefines(Opts, Defines);
  }
};
} // end anonymous namespace

namespace {
// x86-32 Linux target
class LinuxX86_32TargetInfo : public X86_32TargetInfo {
public:
  LinuxX86_32TargetInfo(const std::string& triple) : X86_32TargetInfo(triple) {
    UserLabelPrefix = "";
    SizeType = UnsignedInt;
    PtrDiffType = Int;
  }
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    X86_32TargetInfo::getTargetDefines(Opts, Defines);
    getLinuxDefines(Opts, Defines);
  }
};
} // end anonymous namespace

namespace {
// x86-32 Windows target
class WindowsX86_32TargetInfo : public X86_32TargetInfo {
public:
  WindowsX86_32TargetInfo(const std::string& triple)
    : X86_32TargetInfo(triple) {
    // FIXME: Fix wchar_t.
    // FIXME: We should probably enable -fms-extensions by default for
    // this target.
    SizeType = UnsignedInt;
    PtrDiffType = Int;
  }
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    X86_32TargetInfo::getTargetDefines(Opts, Defines);
    // This list is based off of the the list of things MingW defines
    Define(Defines, "_WIN32");
    DefineStd(Defines, "WIN32", Opts);
    DefineStd(Defines, "WINNT", Opts);
    Define(Defines, "_X86_");
    Define(Defines, "__MSVCRT__");
  }
};
} // end anonymous namespace

namespace {
// x86-64 generic target
class X86_64TargetInfo : public X86TargetInfo {
public:
  X86_64TargetInfo(const std::string &triple) : X86TargetInfo(triple) {
    LongWidth(64); LongAlign(64); PointerWidth(64); PointerAlign(64);
    DoubleAlign(64); LongLongAlign(64);
    LongDoubleWidth(128);
    LongDoubleAlign(128);
    IntMaxType = Long;
    UIntMaxType = UnsignedLong;
    SizeType = UnsignedLong;
    PtrDiffType = Long;
    IntPtrType = Long;
    UIntPtrType = UnsignedLong;
  }
  virtual const char *getVAListDeclaration() const {
    return "typedef char* __builtin_va_list;";
#if RICH
    return "typedef struct __va_list_tag {"
           "  unsigned gp_offset;"
           "  unsigned fp_offset;"
           "  void* overflow_arg_area;"
           "  void* reg_save_area;"
           "} __builtin_va_list[1];";
#endif
  }
};
} // end anonymous namespace

namespace {
// x86-64 FreeBSD target
class FreeBSDX86_64TargetInfo : public X86_64TargetInfo {
public:
  FreeBSDX86_64TargetInfo(const std::string &triple)
    : X86_64TargetInfo(triple) {}
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    X86_64TargetInfo::getTargetDefines(Opts, Defines);
    getFreeBSDDefines(Opts, 1, getTargetTriple(), Defines);
  }
};
} // end anonymous namespace

namespace {
// x86-64 Linux target
class LinuxX86_64TargetInfo : public X86_64TargetInfo {
public:
  LinuxX86_64TargetInfo(const std::string& triple) : X86_64TargetInfo(triple) {
    UserLabelPrefix = "";
  }
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    X86_64TargetInfo::getTargetDefines(Opts, Defines);
    getLinuxDefines(Opts, Defines);
  }
};
} // end anonymous namespace

namespace {
// x86-64 Darwin (OS X) target
class DarwinX86_64TargetInfo : public X86_64TargetInfo {
public:
  DarwinX86_64TargetInfo(const std::string& triple) :
    X86_64TargetInfo(triple) {}

  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    X86_64TargetInfo::getTargetDefines(Opts, Defines);
    getDarwinDefines(Defines, getTargetTriple());
  }

  /// getDefaultLangOptions - Allow the target to specify default settings for
  /// various language options.  These may be overridden by command line
  /// options. 
  virtual void getDefaultLangOptions(LangOptions &Opts) {
    GetDarwinLanguageOptions(Opts, getTargetTriple());
  }
};
} // end anonymous namespace.

namespace {
class ARMTargetInfo : public TargetInfo {
  static const Builtin::Info BuiltinInfo[];
  static const TargetInfo::GCCRegAlias GCCRegAliases[];
  static const char * const GCCRegNames[];
public:
  ARMTargetInfo(const std::string& triple) : TargetInfo(triple) {
  }
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    // Target identification.
    Define(Defines, "__arm__");
    Define(Defines, "__arm");
    
    // Target properties.
    Define(Defines, "__LITTLE_ENDIAN__");
    
    // Subtarget options.  [hard coded to v6 for now]
    Define(Defines, "__ARM_ARCH_6K__");
    Define(Defines, "__ARMEL__");
    Define(Defines, "__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__", "20000");
  }
  virtual void getTargetBuiltins(const Builtin::Info *&Records,
                                 unsigned &NumRecords) const {
    Records = 0; // RICH BuiltinInfo;
    NumRecords = 0; // RICH ellcc::ARM::LastTSBuiltin-Builtin::FirstTSBuiltin;
  }
  virtual const char *getVAListDeclaration() const {
    return "typedef char* __builtin_va_list;";
  }
  virtual const char *getTargetPrefix() const {
    return "arm";
  }
  virtual void getGCCRegNames(const char * const *&Names, 
                              unsigned &NumNames) const;
  virtual void getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                unsigned &NumAliases) const;
  virtual bool validateAsmConstraint(const char *&Name,
                                     TargetInfo::ConstraintInfo &info) const;
  virtual const char *getClobbers() const {
    // FIXME: Is this really right?
    return "";
  }
  virtual void getRaiseInstructionsList(RaiseInstructionsList*& List, 
                                        unsigned &NumRaises) const;
  static RaiseInstructionsList raiseInstructionsList[];
};

TargetInfo::RaiseInstructionsList ARMTargetInfo::raiseInstructionsList[] = {
    { Instruction::URem,        "__umoddi3",   2, { }, { 64, 64, 64 } },
    { Instruction::UDiv,        "__udivdi3",   2, { }, { 64, 64, 64 } },
    { Instruction::AShr,        "__ashrdi3",   2, { }, { 64, 64, 64 } },
    { Instruction::URem,        "__umodsi3",   2, { }, { 32, 32, 32 } },
    { Instruction::UDiv,        "__udivsi3",   2, { }, { 32, 32, 32 } },
};
void ARMTargetInfo::getRaiseInstructionsList(RaiseInstructionsList*& List, 
                                                unsigned &NumRaises) const
{
  List = raiseInstructionsList;
  NumRaises = array_lengthof(raiseInstructionsList);
}

#if RICH
const Builtin::Info BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS, ...) { #ID, TYPE, ATTRS, 0, false, { __VA_ARGS__ } },
#define LIBBUILTIN(ID, TYPE, ATTRS, HEADER, ...) { #ID, TYPE, ATTRS, HEADER, false, { __VA_ARGS__ } },
#include "ARMBuiltins.def"
};
#endif
const char * const ARMTargetInfo::GCCRegNames[] = {
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
};

void ARMTargetInfo::getGCCRegNames(const char * const *&Names, 
                                   unsigned &NumNames) const {
  Names = GCCRegNames;
  NumNames = array_lengthof(GCCRegNames);
}

const TargetInfo::GCCRegAlias ARMTargetInfo::GCCRegAliases[] = {
  { { "a1" }, "r0" }, 
  { { "a2" }, "r1" }, 
  { { "a3" }, "r2" }, 
  { { "a4" }, "r3" }, 
  { { "v1" }, "r4" }, 
  { { "v2" }, "r5" }, 
  { { "v3" }, "r6" }, 
  { { "v4" }, "r7" }, 
  { { "v5" }, "r8" }, 
  { { "v6", "sb" }, "r9" }, 
  { { "v7", "sl" }, "r10" }, 
  { { "v8", "fp" }, "r11" }, 
  { { "ip" }, "r12" }, 
  { { "sp" }, "r13" }, 
  { { "lr" }, "r14" }, 
  { { "pc" }, "r15" }, 
};
void ARMTargetInfo::getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                     unsigned &NumAliases) const {
  Aliases = GCCRegAliases;
  NumAliases = array_lengthof(GCCRegAliases);
}

bool ARMTargetInfo::validateAsmConstraint(const char *&Name,
                                          TargetInfo::ConstraintInfo &info) const
{
    switch (*Name) {
    default: return false;
    case 'l': // r0-r7
    case 'h': // r8-r15
    case 'f': // Floating point register.
    case 'w': // VFP Floating point register single precision
    case 'P': // VFP Floating point register double precision
        info = (TargetInfo::ConstraintInfo)(info|TargetInfo::CI_AllowsRegister);
        return true;
    case 'I': // Integer in the range 0 to 255 rotated by a multiple of 2.
    case 'J': // Integer constant in the range -4095 to 4096.
    case 'K': // Like 'I', but one's complement.
    case 'L': // Like 'I', but two's complement.
    case 'M': // Integer in the range 0 to 32.
    case 'F': // 0.0, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, or 10.0.
    case 'G': // Same as 'F', negated.
        return true;
    }
    return false;
}

} // end anonymous namespace.


namespace {
class DarwinARMTargetInfo : public ARMTargetInfo {
public:
  DarwinARMTargetInfo(const std::string& triple) : ARMTargetInfo(triple) {}

  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    ARMTargetInfo::getTargetDefines(Opts, Defines);
    getDarwinDefines(Defines, getTargetTriple());
  }
};
} // end anonymous namespace.

namespace {
// arm FreeBSD target
class FreeBSDARMTargetInfo : public ARMTargetInfo {
public:
  FreeBSDARMTargetInfo(const std::string& triple) : ARMTargetInfo(triple) {}
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    ARMTargetInfo::getTargetDefines(Opts, Defines);
    getFreeBSDDefines(Opts, 0, getTargetTriple(), Defines);
  }
};
} // end anonymous namespace

namespace {
class SparcV8TargetInfo : public TargetInfo {
  static const TargetInfo::GCCRegAlias GCCRegAliases[];
  static const char * const GCCRegNames[];
public:
  SparcV8TargetInfo(const std::string& triple) : TargetInfo(triple) {
  }
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    // FIXME: This is missing a lot of important defines; some of the
    // missing stuff is likely to break system headers.
    Define(Defines, "__sparc__");
    Define(Defines, "__sparc");
    Define(Defines, "__sparcv8");
 
    // Target properties.
    Define(Defines, "_BIG_ENDIAN");
    Define(Defines, "__BIG_ENDIAN__");
  }
  virtual void getTargetBuiltins(const Builtin::Info *&Records,
                                 unsigned &NumRecords) const {
    // FIXME: Implement!
  }
  virtual const char *getVAListDeclaration() const {
    return "typedef void* __builtin_va_list;";
  }
  virtual const char *getTargetPrefix() const {
    return "sparc";
  }
  virtual void getGCCRegNames(const char * const *&Names, 
                              unsigned &NumNames) const;
  virtual void getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                unsigned &NumAliases) const;
  virtual bool validateAsmConstraint(const char *&Name,
                                     TargetInfo::ConstraintInfo &info) const;
  virtual const char *getClobbers() const {
    // FIXME: Implement!
    return "";
  }
};

const char * const SparcV8TargetInfo::GCCRegNames[] = {
  "g0", "g1", "g2", "g3", "g4", "g5", "g6", "g7",
  "o0", "o1", "o2", "o3", "o4", "o5", "o6", "o7",
  "l0", "l1", "l2", "l3", "l4", "l5", "l6", "l7",
  "i0", "i1", "i2", "i3", "i4", "i5", "i6", "i7",
  "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
  "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15",
  "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
  "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31"
};

void SparcV8TargetInfo::getGCCRegNames(const char * const *&Names, 
                                       unsigned &NumNames) const {
  Names = GCCRegNames;
  NumNames = array_lengthof(GCCRegNames);
}

const TargetInfo::GCCRegAlias SparcV8TargetInfo::GCCRegAliases[] = {
  { { "r0" }, "g0" },
  { { "r1" }, "g1" },
  { { "r2" }, "g2" },
  { { "r3" }, "g3" },
  { { "r4" }, "g4" },
  { { "r5" }, "g5" },
  { { "r6" }, "g6" },
  { { "r7" }, "g7" },
  { { "r8" }, "o0" },
  { { "r9" }, "o1" },
  { { "r10" }, "o2" },
  { { "r11" }, "o3" },
  { { "r12" }, "o4" },
  { { "r13" }, "o5" },
  { { "r14", "sp" }, "o6" },
  { { "r15" }, "o7" },
  { { "r16" }, "l0" },
  { { "r17" }, "l1" },
  { { "r18" }, "l2" },
  { { "r19" }, "l3" },
  { { "r20" }, "l4" },
  { { "r21" }, "l5" },
  { { "r22" }, "l6" },
  { { "r23" }, "l7" },
  { { "r24" }, "i0" },
  { { "r25" }, "i1" },
  { { "r26" }, "i2" },
  { { "r27" }, "i3" },
  { { "r28" }, "i4" },
  { { "r29" }, "i5" },
  { { "r30", "fp" }, "i6" },
  { { "r31" }, "i7" },
};

void SparcV8TargetInfo::getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                         unsigned &NumAliases) const {
  Aliases = GCCRegAliases;
  NumAliases = array_lengthof(GCCRegAliases);
}
} // end anonymous namespace.

namespace {
class SolarisSparcV8TargetInfo : public SparcV8TargetInfo {
public:
  SolarisSparcV8TargetInfo(const std::string& triple) :
      SparcV8TargetInfo(triple) {
    SizeType = UnsignedInt;
    PtrDiffType = Int;
  }

  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const {
    SparcV8TargetInfo::getTargetDefines(Opts, Defines);
    getSolarisDefines(Defines);
  }
};

bool SparcV8TargetInfo::validateAsmConstraint(const char *&Name,
                                              TargetInfo::ConstraintInfo &info) const
{
    switch (*Name) {
    default: return false;
    case 'f': // Floating point register.
    case 'e': // Floating point register.
    case 'c': // Floating point condition code register.
    case 'h': // 64-bit global or out register.
    case 'U': // An even register.
        info = (TargetInfo::ConstraintInfo)(info|TargetInfo::CI_AllowsRegister);
        return true;
    case 'D': // A vector constant.
    case 'I': // A signed 13 bit constant.
    case 'J': // Zero.
    case 'K': // A 32-bit constant with the low 12 bits clear.
    case 'L': // A constant for movcc.
    case 'M': // A constant for movrcc.
    case 'N': // Same as 'K' but bits above 0..31 must be zero.
    case 'O': // 4096.
    case 'G': // Floating point zero.
    case 'H': // A signed 13 bit constant, sign extended to 32 or 64 bits.
    case 'Q': // A floating point constant whose integer representation can be loaded with sethi.
    case 'R': // A floating point constant whose integer representation can be loaded with mov.
    case 'S': // A floating point constant whose integer representation can be loaded with hi/lo_sum.
    case 'T': // 8-byte aligned memory address.
    case 'W': // A memory address for 'e' constraint registers.
    case 'Y': // Vector zero.
        return true;
    }
    return false;
}

} // end anonymous namespace.

namespace {
  class PIC16TargetInfo : public TargetInfo{
  public:
    PIC16TargetInfo(const std::string& triple) : TargetInfo(triple) {
      IntWidth(16);
      LongWidth(32); LongLongWidth(32);
      PointerWidth(16);
      IntAlign(8);
      LongAlign(8); LongLongAlign(8);
      PointerAlign(8);
      SizeType = UnsignedInt;
      IntMaxType = Long;
      UIntMaxType = UnsignedLong;
      IntPtrType = Short;
      UIntPtrType = UnsignedShort;
      PtrDiffType = Int;
    }
    virtual uint64_t getPointerWidthV(unsigned AddrSpace) const { return 16; }
    virtual uint64_t getPointerAlignV(unsigned AddrSpace) const { return 8; }
    virtual void getTargetDefines(const LangOptions &Opts,
                                  std::vector<char> &Defines) const {
      Define(Defines, "__pic16__");
      Define(Defines, "__pic16");
    }
    virtual void getTargetBuiltins(const Builtin::Info *&Records,
                                   unsigned &NumRecords) const {}
    virtual const char *getVAListDeclaration() const { return "";}
    virtual const char *getClobbers() const {return "";}
    virtual const char *getTargetPrefix() const {return "pic16";}
    virtual void getGCCRegNames(const char * const *&Names, 
                                unsigned &NumNames) const {} 
    virtual bool validateAsmConstraint(const char *&Name, 
                                       TargetInfo::ConstraintInfo &info) const;
    virtual void getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                  unsigned &NumAliases) const {}
    virtual bool useGlobalsForAutomaticVariables() const {return true;}
  };

bool PIC16TargetInfo::validateAsmConstraint(const char *&Name, 
                                            TargetInfo::ConstraintInfo &info) const
{
    switch (*Name) {
    default: return false;
    }
}

}

namespace {
// Nios2 target abstract base class.
class Nios2TargetInfo : public TargetInfo {
  static const Builtin::Info BuiltinInfo[];
  static const char * const GCCRegNames[];
public:
  Nios2TargetInfo(const std::string& triple) 
    : TargetInfo(triple) {
  }
  virtual void getTargetBuiltins(const Builtin::Info *&Records,
                                 unsigned &NumRecords) const {
    Records = BuiltinInfo;
    NumRecords = ellcc::Nios2::LastTSBuiltin-Builtin::FirstTSBuiltin;
  }
  virtual const char *getClobbers() const {return "";}
  virtual const char *getTargetPrefix() const {
    return "nios2";
  }  
  virtual void getGCCRegNames(const char * const *&Names, 
                              unsigned &NumNames) const;
  virtual bool validateAsmConstraint(const char *&Name,
                                     TargetInfo::ConstraintInfo &info) const;
  virtual std::string convertConstraint(const char Constraint) const;
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const;
  virtual const char *getVAListDeclaration() const {
    return "typedef char* __builtin_va_list;";
  }
  virtual void getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                unsigned &NumAliases) const {
    // FIXME: Implement.
    Aliases = 0;
    NumAliases = 0;
  }
};

const Builtin::Info Nios2TargetInfo::BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS, ...) { #ID, TYPE, ATTRS, 0, false, { __VA_ARGS__ } },
#define LIBBUILTIN(ID, TYPE, ATTRS, HEADER, ...) { #ID, TYPE, ATTRS, HEADER, false, { __VA_ARGS__ } },
#include "Nios2Builtins.def"
};
  
const char * const Nios2TargetInfo::GCCRegNames[] = {
  "zero", "at", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
  "r9", "r10", "r11", "r12", "r13", "r14", "r15", "r16",
  "r17", "r18", "r19", "r20", "r21", "r22", "r23", "et",
  "bt", "gp", "sp", "fp", "ea", "ba", "ra"
};

void Nios2TargetInfo::getGCCRegNames(const char * const *&Names, 
                                     unsigned &NumNames) const
{
  Names = GCCRegNames;
  NumNames = array_lengthof(GCCRegNames);
}

/// Nios2TargetInfo::getTargetDefines - Return a set of the Nios2-specific #defines
/// that are not tied to a specific subtarget.
void Nios2TargetInfo::getTargetDefines(const LangOptions &Opts,
                                       std::vector<char> &Defines) const {
  // Target identification.
  DefineStd(Defines, "__nios2__", Opts);
  DefineStd(Defines, "nios2", Opts);
  
  // Target properties. FIXME: Big or little.
  Define(Defines, "__LITTLE_ENDIAN__");
  
  // Subtarget options.
  Define(Defines, "__REGISTER_PREFIX__", "");
}
  
// FIXME
bool
Nios2TargetInfo::validateAsmConstraint(const char *&Name,
                                       TargetInfo::ConstraintInfo &info) const
{
    switch (*Name) {
    default: return false;
    }
}

// FIXME
std::string
Nios2TargetInfo::convertConstraint(const char Constraint) const {
  switch (Constraint) {
  case 'a': return std::string("{ax}");
  case 'b': return std::string("{bx}");
  case 'c': return std::string("{cx}");
  case 'd': return std::string("{dx}");
  case 'S': return std::string("{si}");
  case 'D': return std::string("{di}");
  case 't': // top of floating point stack.
    return std::string("{st}");
  case 'u': // second from top of floating point stack.
    return std::string("{st(1)}"); // second from top of floating point stack.
  default:
    return std::string(1, Constraint);
  }
}
} // end anonymous namespace

namespace {
// CellSPU target abstract base class.
class CellSPUTargetInfo : public TargetInfo {
  static const Builtin::Info BuiltinInfo[];
  static const char * const GCCRegNames[];
public:
  CellSPUTargetInfo(const std::string& triple) 
    : TargetInfo(triple) {
  }
  virtual void getTargetBuiltins(const Builtin::Info *&Records,
                                 unsigned &NumRecords) const {
    Records = BuiltinInfo;
    NumRecords = ellcc::CellSPU::LastTSBuiltin-Builtin::FirstTSBuiltin;
  }
  virtual const char *getClobbers() const {return "";}
  virtual const char *getTargetPrefix() const {
    return "cellspu";
  }  
  virtual void getGCCRegNames(const char * const *&Names, 
                              unsigned &NumNames) const;
  virtual bool validateAsmConstraint(const char *&Name,
                                     TargetInfo::ConstraintInfo &info) const;
  virtual std::string convertConstraint(const char Constraint) const;
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const;
  virtual const char *getVAListDeclaration() const {
    return "typedef char* __builtin_va_list;";
  }
  virtual void getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                unsigned &NumAliases) const {
    // FIXME: Implement.
    Aliases = 0;
    NumAliases = 0;
  }
};

const Builtin::Info CellSPUTargetInfo::BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS, ...) { #ID, TYPE, ATTRS, 0, false, { __VA_ARGS__ } },
#define LIBBUILTIN(ID, TYPE, ATTRS, HEADER, ...) { #ID, TYPE, ATTRS, HEADER, false, { __VA_ARGS__ } },
#include "CellSPUBuiltins.def"
};
  
const char * const CellSPUTargetInfo::GCCRegNames[] = {
  "zero", "at", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
  "r9", "r10", "r11", "r12", "r13", "r14", "r15", "r16",
  "r17", "r18", "r19", "r20", "r21", "r22", "r23", "et",
  "bt", "gp", "sp", "fp", "ea", "ba", "ra"
};

void CellSPUTargetInfo::getGCCRegNames(const char * const *&Names, 
                                     unsigned &NumNames) const
{
  Names = GCCRegNames;
  NumNames = array_lengthof(GCCRegNames);
}

/// CellSPUTargetInfo::getTargetDefines - Return a set of the CellSPU-specific #defines
/// that are not tied to a specific subtarget.
void CellSPUTargetInfo::getTargetDefines(const LangOptions &Opts,
                                       std::vector<char> &Defines) const {
  // Target identification.
  DefineStd(Defines, "__cellspu__", Opts);
  DefineStd(Defines, "cellspu", Opts);
  
  // Target properties. FIXME: Big or little.
  Define(Defines, "__LITTLE_ENDIAN__");
  
  // Subtarget options.
  Define(Defines, "__REGISTER_PREFIX__", "");
}
  
bool
CellSPUTargetInfo::validateAsmConstraint(const char *&Name,
                                       TargetInfo::ConstraintInfo &info) const {
  switch (*Name) {
  default: return false;
  case 'a': // eax.
  case 'b': // ebx.
  case 'c': // ecx.
  case 'd': // edx.
  case 'S': // esi.
  case 'D': // edi.
  case 'A': // edx:eax.
  case 't': // top of floating point stack.
  case 'u': // second from top of floating point stack.
  case 'q': // Any register accessible as [r]l: a, b, c, and d.
  case 'y': // Any MMX register.
  case 'x': // Any SSE register.
  case 'Q': // Any register accessible as [r]h: a, b, c, and d.
  case 'e': // 32-bit signed integer constant for use with zero-extending 
            // x86_64 instructions.
  case 'Z': // 32-bit unsigned integer constant for use with zero-extending 
            // x86_64 instructions.
  case 'N': // unsigned 8-bit integer constant for use with in and out
            // instructions.
    info = (TargetInfo::ConstraintInfo)(info|TargetInfo::CI_AllowsRegister);
    return true;
  }
}

// FIXME
std::string
CellSPUTargetInfo::convertConstraint(const char Constraint) const {
  switch (Constraint) {
  case 'a': return std::string("{ax}");
  case 'b': return std::string("{bx}");
  case 'c': return std::string("{cx}");
  case 'd': return std::string("{dx}");
  case 'S': return std::string("{si}");
  case 'D': return std::string("{di}");
  case 't': // top of floating point stack.
    return std::string("{st}");
  case 'u': // second from top of floating point stack.
    return std::string("{st(1)}"); // second from top of floating point stack.
  default:
    return std::string(1, Constraint);
  }
}
} // end anonymous namespace

namespace {
// Mips target abstract base class.
class MipsTargetInfo : public TargetInfo {
  static const Builtin::Info BuiltinInfo[];
  static const char * const GCCRegNames[];
  static const GCCRegAlias GCCRegAliases[];
public:
  MipsTargetInfo(const std::string& triple) 
    : TargetInfo(triple) {
  }
  virtual void getTargetBuiltins(const Builtin::Info *&Records,
                                 unsigned &NumRecords) const {
    Records = BuiltinInfo;
    NumRecords = ellcc::Mips::LastTSBuiltin-Builtin::FirstTSBuiltin;
  }
  virtual const char *getClobbers() const {return "";}
  virtual const char *getTargetPrefix() const {
    return "mips";
  }  
  virtual void getGCCRegNames(const char * const *&Names, 
                              unsigned &NumNames) const;
  virtual bool validateAsmConstraint(const char *&Name,
                                     TargetInfo::ConstraintInfo &info) const;
  virtual std::string convertConstraint(const char Constraint) const;
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const;
  virtual const char *getVAListDeclaration() const {
    return "typedef char* __builtin_va_list;";
  }
  virtual void getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                unsigned &NumAliases) const;
  char getGCCRegPrefix() const { return '$'; };
};

const Builtin::Info MipsTargetInfo::BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS, ...) { #ID, TYPE, ATTRS, 0, false, { __VA_ARGS__ } },
#define LIBBUILTIN(ID, TYPE, ATTRS, HEADER, ...) { #ID, TYPE, ATTRS, HEADER, false, { __VA_ARGS__ } },
#include "MipsBuiltins.def"
};
  
const char * const MipsTargetInfo::GCCRegNames[] = {
  "0", "1", "2", "3", "4", "5", "6", "7", "8",
  "9", "10", "11", "12", "13", "14", "15", "16",
  "17", "18", "19", "20", "21", "22", "23", "24",
  "25", "26", "27", "28", "29", "30", "31"
};

void MipsTargetInfo::getGCCRegNames(const char * const *&Names, 
                                     unsigned &NumNames) const
{
  Names = GCCRegNames;
  NumNames = array_lengthof(GCCRegNames);
}

const TargetInfo::GCCRegAlias MipsTargetInfo::GCCRegAliases[] = {
  { { "zero" }, "0" }, 
  { { "at" }, "1" }, 
  { { "v0" }, "2" }, 
  { { "v1" }, "3" }, 
  { { "a0" }, "4" }, 
  { { "a1" }, "5" }, 
  { { "a2" }, "6" }, 
  { { "a3" }, "7" }, 
  { { "t0" }, "8" }, 
  { { "t1" }, "9" }, 
  { { "t2" }, "10" }, 
  { { "t3" }, "11" }, 
  { { "t4" }, "12" }, 
  { { "t5" }, "13" }, 
  { { "t6" }, "14" }, 
  { { "t7" }, "15" }, 
  { { "s0" }, "16" }, 
  { { "s1" }, "17" }, 
  { { "s2" }, "18" }, 
  { { "s3" }, "19" }, 
  { { "s4" }, "20" }, 
  { { "s5" }, "21" }, 
  { { "s6" }, "22" }, 
  { { "s7" }, "23" }, 
  { { "t8" }, "24" }, 
  { { "t9" }, "25" }, 
  { { "k0" }, "26" }, 
  { { "k1" }, "27" }, 
  { { "gp" }, "28" }, 
  { { "sp" }, "29" }, 
  { { "fp", "s8" }, "29" }, 
  { { "ra" }, "31" }, 
};
void MipsTargetInfo::getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                     unsigned &NumAliases) const {
  Aliases = GCCRegAliases;
  NumAliases = array_lengthof(GCCRegAliases);
}

/// MipsTargetInfo::getTargetDefines - Return a set of the Mips-specific #defines
/// that are not tied to a specific subtarget.
void MipsTargetInfo::getTargetDefines(const LangOptions &Opts,
                                       std::vector<char> &Defines) const {
  // Target identification.
  DefineStd(Defines, "__mips__", Opts);
  DefineStd(Defines, "mips", Opts);
  
  // Target properties. FIXME: Big or little.
  Define(Defines, "__LITTLE_ENDIAN__");
  
  // Subtarget options.
  Define(Defines, "__REGISTER_PREFIX__", "");
}
  
// FIXME
bool
MipsTargetInfo::validateAsmConstraint(const char *&Name,
                                       TargetInfo::ConstraintInfo &info) const {
  switch (*Name) {
  default: return false;
  case 'd': // An address register.
  case 'f': // A floating point register.
  case 'h': // The hi register..
  case 'l': // The lo register.
  case 'x': // the hi and lo registers.
  case 'c': // An indirect jump register.
  case 'y': // Backward compatability: equivalent to 'r'.
  case 'z': // A floating point condition code register.
    info = (TargetInfo::ConstraintInfo)(info|TargetInfo::CI_AllowsRegister);
    return true;
  }
}

// FIXME
std::string
MipsTargetInfo::convertConstraint(const char Constraint) const {
  switch (Constraint) {
  case 'a': return std::string("{ax}");
  case 'b': return std::string("{bx}");
  case 'c': return std::string("{cx}");
  case 'd': return std::string("{dx}");
  case 'S': return std::string("{si}");
  case 'D': return std::string("{di}");
  case 't': // top of floating point stack.
    return std::string("{st}");
  case 'u': // second from top of floating point stack.
    return std::string("{st(1)}"); // second from top of floating point stack.
  default:
    return std::string(1, Constraint);
  }
}
} // end anonymous namespace

namespace {
// Msp430 target abstract base class.
class Msp430TargetInfo : public TargetInfo {
  static const Builtin::Info BuiltinInfo[];
  static const char * const GCCRegNames[];
public:
  Msp430TargetInfo(const std::string& triple) 
    : TargetInfo(triple) {
  }
  virtual void getTargetBuiltins(const Builtin::Info *&Records,
                                 unsigned &NumRecords) const {
    Records = BuiltinInfo;
    NumRecords = ellcc::Msp430::LastTSBuiltin-Builtin::FirstTSBuiltin;
  }
  virtual const char *getClobbers() const {return "";}
  virtual const char *getTargetPrefix() const {
    return "msp430";
  }  
  virtual void getGCCRegNames(const char * const *&Names, 
                              unsigned &NumNames) const;
  virtual bool validateAsmConstraint(const char *&Name,
                                     TargetInfo::ConstraintInfo &info) const;
  virtual std::string convertConstraint(const char Constraint) const;
  virtual void getTargetDefines(const LangOptions &Opts,
                                std::vector<char> &Defines) const;
  virtual const char *getVAListDeclaration() const {
    return "typedef char* __builtin_va_list;";
  }
  virtual void getGCCRegAliases(const GCCRegAlias *&Aliases, 
                                unsigned &NumAliases) const {
    // FIXME: Implement.
    Aliases = 0;
    NumAliases = 0;
  }
};

const Builtin::Info Msp430TargetInfo::BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS, ...) { #ID, TYPE, ATTRS, 0, false, { __VA_ARGS__ } },
#define LIBBUILTIN(ID, TYPE, ATTRS, HEADER, ...) { #ID, TYPE, ATTRS, HEADER, false, { __VA_ARGS__ } },
#include "Msp430Builtins.def"
};
  
const char * const Msp430TargetInfo::GCCRegNames[] = {
  "zero", "at", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
  "r9", "r10", "r11", "r12", "r13", "r14", "r15", "r16",
  "r17", "r18", "r19", "r20", "r21", "r22", "r23", "et",
  "bt", "gp", "sp", "fp", "ea", "ba", "ra"
};

void Msp430TargetInfo::getGCCRegNames(const char * const *&Names, 
                                     unsigned &NumNames) const
{
  Names = GCCRegNames;
  NumNames = array_lengthof(GCCRegNames);
}

/// Msp430TargetInfo::getTargetDefines - Return a set of the Msp430-specific #defines
/// that are not tied to a specific subtarget.
void Msp430TargetInfo::getTargetDefines(const LangOptions &Opts,
                                       std::vector<char> &Defines) const {
  // Target identification.
  DefineStd(Defines, "__msp430__", Opts);
  DefineStd(Defines, "msp430", Opts);
  
  // Target properties. FIXME: Big or little.
  Define(Defines, "__LITTLE_ENDIAN__");
  
  // Subtarget options.
  Define(Defines, "__REGISTER_PREFIX__", "");
}
  
// FIXME
bool
Msp430TargetInfo::validateAsmConstraint(const char *&Name,
                                       TargetInfo::ConstraintInfo &info) const {
  switch (*Name) {
  default: return false;
  case 'a': // eax.
  case 'b': // ebx.
  case 'c': // ecx.
  case 'd': // edx.
  case 'S': // esi.
  case 'D': // edi.
  case 'A': // edx:eax.
  case 't': // top of floating point stack.
  case 'u': // second from top of floating point stack.
  case 'q': // Any register accessible as [r]l: a, b, c, and d.
  case 'y': // Any MMX register.
  case 'x': // Any SSE register.
  case 'Q': // Any register accessible as [r]h: a, b, c, and d.
  case 'e': // 32-bit signed integer constant for use with zero-extending 
            // x86_64 instructions.
  case 'Z': // 32-bit unsigned integer constant for use with zero-extending 
            // x86_64 instructions.
  case 'N': // unsigned 8-bit integer constant for use with in and out
            // instructions.
    info = (TargetInfo::ConstraintInfo)(info|TargetInfo::CI_AllowsRegister);
    return true;
  }
}

// FIXME
std::string
Msp430TargetInfo::convertConstraint(const char Constraint) const {
  switch (Constraint) {
  case 'a': return std::string("{ax}");
  case 'b': return std::string("{bx}");
  case 'c': return std::string("{cx}");
  case 'd': return std::string("{dx}");
  case 'S': return std::string("{si}");
  case 'D': return std::string("{di}");
  case 't': // top of floating point stack.
    return std::string("{st}");
  case 'u': // second from top of floating point stack.
    return std::string("{st(1)}"); // second from top of floating point stack.
  default:
    return std::string(1, Constraint);
  }
}
} // end anonymous namespace

//===----------------------------------------------------------------------===//
// Driver code
//===----------------------------------------------------------------------===//

static inline bool IsX86(const std::string& TT) {
  return (TT.size() >= 5 && TT[0] == 'i' && TT[2] == '8' && TT[3] == '6' &&
          TT[4] == '-' && TT[1] - '3' < 6);
}

/// CreateTargetInfo - Return the target info object for the specified target
/// triple.
TargetInfo* TargetInfo::CreateTargetInfo(const std::string &T) {
  // OS detection; this isn't really anywhere near complete.
  // Additions and corrections are welcome.
  bool isDarwin = T.find("-darwin") != std::string::npos;
  bool isDragonFly = T.find("-dragonfly") != std::string::npos;
  bool isFreeBSD = T.find("-freebsd") != std::string::npos;
  bool isSolaris = T.find("-solaris") != std::string::npos;
  bool isLinux = T.find("-linux") != std::string::npos;
  bool isWindows = T.find("-windows") != std::string::npos ||
                   T.find("-win32") != std::string::npos ||
                   T.find("-mingw") != std::string::npos;

  if (T.find("alpha-") == 0)
    return new AlphaTargetInfo(T);

  if (T.find("ppc-") == 0 || T.find("powerpc-") == 0) {
    if (isDarwin)
      return new DarwinPPCTargetInfo(T);
    return new PPC32TargetInfo(T);
  }

  if (T.find("ppc64-") == 0 || T.find("powerpc64-") == 0) {
    if (isDarwin)
      return new DarwinPPC64TargetInfo(T);
    return new PPC64TargetInfo(T);
  }

  if (T.find("armv6-") == 0 || T.find("arm-") == 0) {
    if (isFreeBSD)
      return new FreeBSDARMTargetInfo(T);
    if (isDarwin)
      return new DarwinARMTargetInfo(T);
    return new ARMTargetInfo(T);
  }

  if (T.find("sparc-") == 0) {
    if (isSolaris)
      return new SolarisSparcV8TargetInfo(T);
    return new SparcV8TargetInfo(T);
  }

  if (T.find("x86_64-") == 0 || T.find("amd64-") == 0) {
    if (isDarwin)
      return new DarwinX86_64TargetInfo(T);
    if (isLinux)
      return new LinuxX86_64TargetInfo(T);
    if (isFreeBSD)
      return new FreeBSDX86_64TargetInfo(T);
    return new X86_64TargetInfo(T);
  }

  if (T.find("pic16-") == 0)
    return new PIC16TargetInfo(T);

  if (IsX86(T) || T.find("x86-") == 0) {
    if (isDarwin)
      return new DarwinI386TargetInfo(T);
    if (isLinux)
      return new LinuxX86_32TargetInfo(T);
    if (isDragonFly)
      return new DragonFlyX86_32TargetInfo(T);
    if (isFreeBSD)
      return new FreeBSDX86_32TargetInfo(T);
    if (isWindows)
      return new WindowsX86_32TargetInfo(T);
    return new X86_32TargetInfo(T);
  }

  if (T.find("nios2-") == 0)
    return new Nios2TargetInfo(T);

  if (T.find("cellspu-") == 0)
    return new CellSPUTargetInfo(T);

  if (T.find("mips-") == 0)
    return new MipsTargetInfo(T);

  if (T.find("msp430-") == 0)
    return new Msp430TargetInfo(T);

  return NULL;
}

