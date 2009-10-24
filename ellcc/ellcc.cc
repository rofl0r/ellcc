//===- ellcc.cc - The Elsa<->LLVM driver ----------------------------------===//
//
// Copyright (c) 2009, Richard Pennington
// All rights reserved.
//
//===----------------------------------------------------------------------===//

#include "EllccDiagnostic.h"
#include "ElsaDiagnostic.h"
#include "LexDiagnostic.h"
#include "HeaderSearch.h"
#include "InitHeaderSearch.h"
#include "Preprocessor.h"
#include "InitPreprocessor.h"
#include "FileManager.h"
#include "TextDiagnosticPrinter.h"
#include "LangOptions.h"
#include "raiseinstructions.h"
#include "TargetInfo.h"
#include "StandardPasses.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/ModuleProvider.h"
#include "llvm/PassManager.h"
#include "llvm/CallGraphSCCPass.h"
#include "llvm/Instruction.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Target/SubtargetFeature.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/FileWriters.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/PassNameParser.h"
#include "llvm/System/Signals.h"
#include "llvm/System/Host.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/RegistryParser.h"
#include "llvm/Support/Streams.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/LinkAllVMCore.h"
#include "llvm/Linker.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>
#include <unistd.h>

#include "ellcc.h"  

#define xxstr(x) #x
#define xstr(x) xxstr(x)

#define ELLCC 0
#define ELLCC_MINOR 1
#define ELLCC_PATCHLEVEL 0
#define ELLCC_VERSION_STRING xstr(ELLCC) "." xstr(ELLCC_MINOR) "." xstr(ELLCC_PATCHLEVEL)
#define ELLCC_VERSION_MODIFIER "ALPHA"
#define ELLCC_VERSION ELLCC_VERSION_STRING " " ELLCC_VERSION_MODIFIER " " __DATE__

using namespace llvm;
using namespace ellcc;

static std::string progname;                    // The program name.        
static const char* argv0;                       // argv[0]
sys::Path PrefixPath;                             // The "prefix" path.
static OwningPtr<SourceManager> SourceMgr;
// Create a file manager object to provide access to and cache the filesystem.
static FileManager FileMgr;
static Diagnostic Diags;
static OwningPtr<TargetInfo> TI;
static std::vector<const char *> exportList;   // Externals to preserve.
 
/** File types.
 */
enum FileTypes {
  NONE,                         ///< Not used.
  C,                            ///< C. 
  I,                            ///< Preprocessed C.
  H,                            ///< Header file.
  CC,                           ///< C++.
  II,                           ///< Preprocessed C++.
  HH,                           ///< A C++ header file.
  LL,                           ///< An LLVM assembly file.
  LLX,                          ///< An LLVM assembly file that needs preprocessing.
  UBC,                          ///< An unoptimized LLVM bitcode file.
  BC,                           ///< An LLVM bitcode file.
  S,                            ///< A native assembly source file.
  SX,                           ///< A native assembly file that needs preprocessing.
  O,                            ///< Other: An object file, linker command file, etc.
  A,                            ///< A library.
  EXE,                          ///< An executable file.
  LINKED,                       ///< A file that has been linked.
  DLL,                          ///< A dynamic library.
  CBE,                          ///< A C backend output file.
  AA,                           ///< A library specified with -l
  NUM_FILE_TYPES                ///< Always last!
};

static const char* fileTypes[] = {
  "NONE",
  "a C file",
  "a preprocessed C file",
  "a header file file",
  "a C++ file",
  "a preprocessed C++ file",
  "a C++ header",
  "an LLVM assembly file",
  "an LLVM assembly file that needs preprocessing",
  "an unoptimized LLVM bitcode file",
  "an LLVM bitcode file",
  "an assembly source file",
  "an assembly file that needs preprocessing",
  "an object file",
  "a library",
  "an executable file",
  "a file that has been linked",
  "a dynamic library", 
  "a C backend output file",
  "a library specified with -l",
};

/** Translation phases.
 */
enum Phases {
    PREPROCESSING,              ///< Source language combining, filtering, substitution
    TRANSLATION,                ///< Translate source -> LLVM bitcode/assembly
    OPTIMIZATION,               ///< Optimize translation result
    BCLINKING,                  ///< Link and optimize bitcode files. 
    GENERATION,                 ///< Convert .bc to .s
    ASSEMBLY,                   ///< Convert .s to .o
    LINKING,                    ///< Link and create executable
    NUM_PHASES                  ///< Always last!
};


static const struct {
    const char* name;           ///< The name of the action.
    FileTypes result;           ///< The result of this phase if it processes multiple files.
} phases[] = {
    { "Preprocessing", },
    { "Translation", },
    { "Optimization", },
    { "Bitcode linking", BC },
    { "Generating", },
    { "Assembly", },
    { "Linking", EXE }
};

static Timer* timers[NUM_PHASES];       // The phase timers.

/** File types by extension.
 */
static std::map<std::string, FileTypes> extToLang;

/** File extensions by language.
 */
static std::map<FileTypes, std::string> langToExt;

/** Actions that can be taken on a file.
 */
enum FileActions {
    PREPROCESS,                 ///> Preprocess a file.
    CCOMPILE,                   ///> Compile a C/C++ program.
    LLASSEMBLE,                 ///> Assemble an LLVM assembly file.
    OPTIMIZE,                   ///> Optimize an LLVM bitcode file.
    BCLINK,                     ///> Link LLVM bitcode files.
    GENERATE,                   ///> Convert .bc to .s.
    ASSEMBLE,                   ///> Assemble a .s file.
    LINK,                       ///> Link object files files.
    NUM_ACTIONS                 ///> Always last!
};

static const struct {
    const char* name;           ///< The name of the action.
} fileActions[] = {
    { "preprocess" },
    { "compile" },
    { "llassemble" },
    { "optimize" },
    { "bclink", },
    { "generate" },
    { "assemble" },
    { "link", },
};

/** Association of file types, actions, and phases.
 */
struct FileInfo {
    FileTypes type;
    FileActions action;
};
static FileInfo filePhases[NUM_FILE_TYPES][NUM_PHASES];

/** Convert output machine to march.
 */
static std::map<std::string, std::string> machines;

/** Convert output machine to linker emulation mode.
 */
static std::map<std::string, std::string> emulations;

/** Set up the file type map.
 */
static void setupMappings()
{
    machines["alpha"] = "alpha";
    machines["arm"] = "arm";
    machines["mips"] = "mips";
    machines["msp430"] = "msp430";
    machines["nios2"] = "nios2";
    machines["powerpc"] = "powerpc";
    machines["powerpc64"] = "powerpc64";
    machines["ppc"] = "powerpc";
    machines["ppc64"] = "powerpc64";
    machines["sparc"] = "sparc";
    machines["spu"] = "cellspu";
    machines["x86"] = "x86";
    machines["x86_64"] = "x86_64";

    emulations["alpha"] = "elf64alpha";
    emulations["arm"] = "armelf";
    emulations["mips"] = "elf32ebmip";
    emulations["msp430"] = "msp430";
    emulations["nios2"] = "nios2elf";
    emulations["powerpc"] = "elf32ppc";
    emulations["powerpc64"] = "elf64ppc";
    emulations["sparc"] = "elf32_sparc";
    emulations["spu"] = "elf32_spu";
    emulations["x86"] = "elf_i386";
    emulations["x86_64"] = "elf_x86_64";
    
    // The extension to language and language to phase mapping.
    extToLang["c"] = C;
    langToExt[C] = "c";
    filePhases[C][TRANSLATION].type = UBC;
    filePhases[C][TRANSLATION].action = CCOMPILE;

    extToLang["i"] = I;                                 // A preprocessed C file becomes a unoptimized bitcode file.
    langToExt[I] = "i";
    filePhases[I][TRANSLATION].type = UBC;
    filePhases[I][TRANSLATION].action = CCOMPILE;

    extToLang["h"] = H;
    langToExt[H] = "h";
    filePhases[H][PREPROCESSING].type = I;
    filePhases[H][PREPROCESSING].action = PREPROCESS;

    extToLang["cc"] = CC;
    langToExt[CC] = "cc";
    filePhases[CC][TRANSLATION].type = UBC;
    filePhases[CC][TRANSLATION].action = CCOMPILE;
    extToLang["cp"] = CC;
    extToLang["cxx"] = CC;
    extToLang["cpp"] = CC;
    extToLang["c++"] = CC;
    extToLang["C"] = CC;

    extToLang["ii"] = II;
    langToExt[II] = "ii";
    filePhases[II][TRANSLATION].type = UBC;
    filePhases[II][TRANSLATION].action = CCOMPILE;

    extToLang["hh"] = HH;
    langToExt[HH] = "hh";
    filePhases[HH][PREPROCESSING].type = II;
    filePhases[HH][PREPROCESSING].action = PREPROCESS;
    extToLang["H"] = HH;
    extToLang["hp"] = HH;
    extToLang["hxx"] = HH;
    extToLang["h++"] = HH;
    extToLang["tcc"] = HH;

    extToLang["ll"] = LL;
    langToExt[LL] = "ll";
    filePhases[LL][TRANSLATION].type = UBC;
    filePhases[LL][TRANSLATION].action = LLASSEMBLE;

    extToLang["llx"] = LLX;
    langToExt[LLX] = "llx";
    filePhases[LLX][PREPROCESSING].type = LL;
    filePhases[LLX][PREPROCESSING].action = PREPROCESS;

    extToLang["ubc"] = UBC;
    langToExt[UBC] = "ubc";
    filePhases[UBC][OPTIMIZATION].type = O;
    filePhases[UBC][OPTIMIZATION].action = OPTIMIZE;

    extToLang["bc"] = BC;
    langToExt[BC] = "bc";
    filePhases[BC][BCLINKING].type = LINKED;
    filePhases[BC][BCLINKING].action = BCLINK;
    filePhases[BC][GENERATION].type = S;
    filePhases[BC][GENERATION].action = GENERATE;

    extToLang["s"] = S;
    langToExt[S] = "s";
    filePhases[S][ASSEMBLY].type = O;
    filePhases[S][ASSEMBLY].action = ASSEMBLE;

    extToLang["sx"] = SX;
    langToExt[SX] = "s";
    filePhases[SX][PREPROCESSING].type = S;
    filePhases[SX][PREPROCESSING].action = PREPROCESS;
    extToLang["S"] = SX;

    extToLang["o"] = O;
    langToExt[O] = "o";
    filePhases[O][BCLINKING].type = LINKED;
    filePhases[O][BCLINKING].action = BCLINK;
    filePhases[O][LINKING].type = LINKED;
    filePhases[O][LINKING].action = LINK;
    filePhases[O][GENERATION].type = S;
    filePhases[O][GENERATION].action = GENERATE;

    extToLang["a"] = A;
    langToExt[A] = "a";
    filePhases[A][BCLINKING].type = LINKED;
    filePhases[A][BCLINKING].action = BCLINK;
    filePhases[A][LINKING].type = A;
    filePhases[A][LINKING].action = LINK;

    filePhases[AA][BCLINKING].type = LINKED;
    filePhases[AA][BCLINKING].action = BCLINK;
    filePhases[AA][LINKING].type = AA;
    filePhases[AA][LINKING].action = LINK;
}

//===----------------------------------------------------------------------===//
// Global options.
//===----------------------------------------------------------------------===//

static cl::opt<bool>
EmptyInputOnly("empty-input-only", 
      cl::desc("Force running on an empty input file"));

//===----------------------------------------------------------------------===//
// Diagnostic Options
//===----------------------------------------------------------------------===//

static cl::opt<bool>
NoShowColumn("fno-show-column",
             cl::desc("Do not include column number on diagnostics"));

static cl::opt<bool>
NoShowLocation("fno-show-source-location",
               cl::desc("Do not include source location information with"
                              " diagnostics"));

static cl::opt<bool>
NoCaretDiagnostics("fno-caret-diagnostics",
                   cl::desc("Do not include source line and caret with"
                                  " diagnostics"));

static cl::opt<bool>
NoDiagnosticsFixIt("fno-diagnostics-fixit-info",
                   cl::desc("Do not include fixit information in"
                                  " diagnostics"));

static cl::opt<bool>
PrintSourceRangeInfo("fdiagnostics-print-source-range-info",
                     cl::desc("Print source range spans in numeric form"));

static cl::opt<bool>
PrintDiagnosticOption("fdiagnostics-show-option",
             cl::desc("Print diagnostic name with mappable diagnostics"));

static cl::opt<unsigned>
MessageLength("fmessage-length",
	      cl::desc("Format message diagnostics so that they fit "
			     "within N columns or fewer, when possible."),
	      cl::value_desc("N"));

static cl::opt<bool>
NoColorDiagnostic("fno-color-diagnostics",
	      cl::desc("Don't use colors when showing diagnostics "
                             "(automatically turned off if output is not a "
                             "terminal)."));

//===----------------------------------------------------------------------===//
//===          PHASE OPTIONS
//===----------------------------------------------------------------------===//

static cl::opt<Phases> FinalPhase(cl::Optional,
    cl::desc("Choose final phase of translation:"),
    cl::init(LINKING),
    cl::values(
        clEnumValN(PREPROCESSING,"E",
            "Stop translation after pre-processing phase"),
        clEnumValN(TRANSLATION, "t",
            "Stop translation after translation phase"),
        clEnumValN(OPTIMIZATION,"c",
            "Stop translation after optimization phase"),
        clEnumValN(BCLINKING,"lbc",
            "Stop translation after bitcode linking phase"),
        clEnumValN(GENERATION,"S",
            "Stop translation after generation phase"),
        clEnumValEnd
    )
);

static cl::opt<bool>
ParseOnly("PO", cl::desc("Parse only, do not translate"));

static cl::opt<bool>
NoLink("no-link", cl::desc("Do not link bitcode files (for -c and -S)"));

static cl::opt<bool>
NoCrt0("no-crt0", cl::desc("Do not link the startup code (for _start) files"));
//
//===----------------------------------------------------------------------===//
//===          OPTIMIZATION OPTIONS
//===----------------------------------------------------------------------===//

/** Optimization levels.
 */
enum OptimizationLevels {
    OPT_NONE,                   ///< No optimizations.
    OPT_FAST_COMPILE,           ///< Optimize to make >compile< go faster
    OPT_SIMPLE,                 ///< Standard/simple optimizations
    OPT_AGGRESSIVE,             ///< Aggressive optimizations
    OPT_LINK_TIME,              ///< Aggressive + LinkTime optimizations
    OPT_AGGRESSIVE_LINK_TIME    ///< Make it go way fast!
};

static cl::opt<OptimizationLevels> OptLevel(cl::ZeroOrMore,
  cl::desc("Choose level of optimization to apply:"),
  cl::init(OPT_FAST_COMPILE),
  cl::values(
    clEnumValN(OPT_FAST_COMPILE,"O",
      "An alias for the -O1 option"),
    clEnumValN(OPT_NONE,"O0",
      "Perform no optimizations"),
    clEnumValN(OPT_FAST_COMPILE,"O1",
      "Optimize for compilation speed, not execution speed"),
    clEnumValN(OPT_FAST_COMPILE,"std-compile-opts",
      "An alias for the -O1 option"),
    clEnumValN(OPT_SIMPLE,"O2",
      "Perform simple translation time optimizations"),
    clEnumValN(OPT_AGGRESSIVE,"O3",
      "Perform aggressive translation time optimizations"),
    clEnumValN(OPT_LINK_TIME,"O4",
      "Perform link time optimizations"),
    clEnumValN(OPT_AGGRESSIVE_LINK_TIME,"O5",
      "Perform aggressive link time optimizations"),
    clEnumValEnd
  )
);

static CodeGenOpt::Level getCodeGenOpt()
{
    switch (OptLevel) {
    default:
    case OPT_FAST_COMPILE:
        return CodeGenOpt::None;
    case OPT_SIMPLE:
        return CodeGenOpt::Default;
    case OPT_AGGRESSIVE:
    case OPT_LINK_TIME:
    case OPT_AGGRESSIVE_LINK_TIME:
        return CodeGenOpt::Aggressive;
    }
}

static cl::opt<bool> DisableInline("disable-inlining",
  cl::desc("Do not run the inliner pass"));

static cl::opt<bool> DisableInternalize("disable-internalize",
  cl::desc("Do not mark all symbols as internal"));

static cl::opt<bool> VerifyEach("verify-each",
 cl::desc("Verify intermediate results of all passes"));

static cl::alias ExportDynamic("export-dynamic",
  cl::aliasopt(DisableInternalize),
  cl::desc("Alias for -disable-internalize"));

static cl::opt<bool> Strip("strip-all", 
  cl::desc("Strip all symbol info from executable"));

static cl::alias A0("s", cl::desc("Alias for --strip-all"), 
  cl::aliasopt(Strip));

static cl::opt<bool> StripDebug("strip-debug",
  cl::desc("Strip debugger symbol info from executable"));

static cl::alias A1("SD", cl::desc("Alias for --strip-debug"),
  cl::aliasopt(StripDebug));

static cl::opt<bool> KeepInlineFunctions("fkeep-inline-functions", 
  cl::desc("Keep static inline functions"));            // RICH
//
//===----------------------------------------------------------------------===//
//===          TOOL OPTIONS
//===----------------------------------------------------------------------===//

static cl::list<std::string> PreprocessorToolOpts("Tpre", cl::ZeroOrMore,
    cl::desc("Pass specific options to the pre-processor"),
    cl::value_desc("option"));

static cl::alias PreprocessorToolOptsAlias("Wp,", cl::ZeroOrMore,
    cl::desc("Alias for -Tpre"), cl::aliasopt(PreprocessorToolOpts));

static cl::list<std::string> TranslatorToolOpts("Ttrn", cl::ZeroOrMore,
    cl::desc("Pass specific options to the assembler"),
    cl::value_desc("option"));

static cl::list<std::string> AssemblerToolOpts("Tasm", cl::ZeroOrMore,
    cl::desc("Pass specific options to the assembler"),
    cl::value_desc("option"));

static cl::alias AssemblerToolOptsAlias("Wa,", cl::ZeroOrMore,
    cl::desc("Alias for -Tasm"), cl::aliasopt(AssemblerToolOpts));

static cl::list<std::string> OptimizerToolOpts("Topt", cl::ZeroOrMore,
    cl::desc("Pass specific options to the optimizer"),
    cl::value_desc("option"));

static cl::list<std::string> LinkerToolOpts("Tlnk", cl::ZeroOrMore,
    cl::desc("Pass specific options to the linker"),
    cl::value_desc("option"));

static cl::alias LinkerToolOptsAlias("Wl,", cl::ZeroOrMore,
    cl::desc("Alias for -Tlnk"), cl::aliasopt(LinkerToolOpts));

#if RICH
static cl::list<std::string> fOpts("f", cl::ZeroOrMore, cl::Prefix,
    cl::desc("Pass through -f options to compiler tools"),
    cl::value_desc("option"));
#endif

static cl::list<std::string> MOpts("M", cl::ZeroOrMore, cl::Prefix,
    cl::desc("Pass through -M options to compiler tools"),
    cl::value_desc("option"));

static cl::list<std::string> BOpt("B", cl::ZeroOrMore, cl::Prefix,
    cl::desc("Specify path to find ellcc sub-tools"),
    cl::value_desc("dir"));

//===----------------------------------------------------------------------===//
//===          INPUT OPTIONS
//===----------------------------------------------------------------------===//

static cl::list<std::string> LibPaths("L", cl::Prefix,
    cl::desc("Specify a library search path"), cl::value_desc("dir"));

static cl::list<std::string> Libraries("l", cl::Prefix,
    cl::desc("Specify base name of libraries to link to"), cl::value_desc("lib"));

//===----------------------------------------------------------------------===//
//===          OUTPUT OPTIONS
//===----------------------------------------------------------------------===//

static cl::opt<std::string> OutputFilename("o", cl::init(""),
    cl::desc("Override output filename"), cl::value_desc("file"));

static cl::opt<bool> Native("native", cl::init(false),
    cl::desc("Generative native code instead of bitcode"));

static cl::opt<bool> DebugOutput("g", cl::init(false), cl::ZeroOrMore,
    cl::desc("Generate objects that include debug symbols"));

#if RICH
static cl::opt<bool> StripOutput("strip", cl::init(false),
    cl::desc("Strip all symbols from linked output file"));
#endif

static cl::opt<std::string> PrintFileName("print-fname", cl::Optional,
    cl::value_desc("file"),
    cl::desc("Print the full path for the option's value"));

//===----------------------------------------------------------------------===//
//===          INFORMATION OPTIONS
//===----------------------------------------------------------------------===//

static cl::opt<bool> DryRun("dry-run", cl::Optional, cl::init(false),
    cl::desc("Do everything but perform the compilation actions"));

static cl::alias DryRunAlias("y", cl::Optional,
    cl::desc("Alias for -dry-run"), cl::aliasopt(DryRun));

static cl::opt<bool> Verbose("verbose", cl::Optional, cl::init(false),
    cl::desc("Print out each action taken"));

static cl::alias VerboseAlias("v", cl::Optional,
    cl::desc("Alias for -verbose"), cl::aliasopt(Verbose));

static cl::opt<bool> TimeActions("time-actions", cl::Optional, cl::init(false),
    cl::desc("Print execution time for each action taken"));

#if RICH
static cl::opt<bool> ShowStats("stats", cl::Optional, cl::init(false),
    cl::desc("Print statistics accumulated during optimization"));
#endif

//===----------------------------------------------------------------------===//
//===          ELSA OPTIONS
//===----------------------------------------------------------------------===//

static cl::list<std::string> ElsaTraceOpts("tr", cl::ZeroOrMore,
    cl::desc("Elsa trace options"),
    cl::value_desc("option"));
static cl::opt<bool> ElsaPrettyPrint("bpprint", cl::Optional, cl::init(false),
    cl::desc("Output pretty printed source"));
static cl::opt<bool> ElsaPrettyPrintAfterElab("bpprintAfterElab", cl::Optional, cl::init(false),
    cl::desc("Output pretty printed source after elaboration"));
static cl::opt<bool> Fno_builtin("fno-builtin", cl::Optional, cl::init(false),   // RICH
    cl::desc("gcc compatability option"));
static cl::opt<bool> Fshort_enums("fshort-enums", cl::Optional, cl::init(false), // RICH
    cl::desc("gcc compatability option"));
static cl::opt<bool> Ffunction_sections("ffunction-sections", cl::Optional, cl::init(false),     // RICH
    cl::desc("gcc compatability option"));
static cl::opt<bool> Fdata_sections("fdata-sections", cl::Optional, cl::init(false),     // RICH
    cl::desc("gcc compatability option"));
static cl::opt<bool> Mno_eabi("mno-eabi", cl::Optional, cl::init(false),     // RICH
    cl::desc("gcc compatability option"));
static cl::opt<bool> Mstrict_align("mstrict-align", cl::Optional, cl::init(false),     // RICH
    cl::desc("gcc compatability option"));
static cl::opt<bool> Mrelocatable_lib("mrelocatable-lib", cl::Optional, cl::init(false),     // RICH
    cl::desc("gcc compatability option"));

//===----------------------------------------------------------------------===//
//===          LINKER OPTIONS
//===----------------------------------------------------------------------===//

// Options to control the linking, optimization, and code gen processes
static cl::opt<bool> LinkAsLibrary("link-as-library",
  cl::desc("Link the .bc files together as a library, not an executable"));

static cl::alias Relink("r", cl::aliasopt(LinkAsLibrary),
  cl::desc("Alias for -link-as-library"));

static cl::opt<bool>NativeCBE("native-cbe",
  cl::desc("Generate a native binary with the C backend and GCC"));

static cl::list<std::string> PostLinkOpts("post-link-opts",
  cl::value_desc("path"),
  cl::desc("Run one or more optimization programs after linking"));

static cl::list<std::string> XLinker("Xlinker", cl::value_desc("option"),
  cl::desc("Pass options to the system linker"));

// Compatibility options that ellcc ignores but are supported for
// compatibility with LD
static cl::opt<std::string> CO3("soname", cl::Hidden,
  cl::desc("Compatibility option: ignored"));

static cl::opt<std::string> CO4("version-script", cl::Hidden,
  cl::desc("Compatibility option: ignored"));

static cl::opt<bool> CO5("eh-frame-hdr", cl::Hidden,
  cl::desc("Compatibility option: ignored"));

static  cl::opt<std::string> CO6("h", cl::Hidden,
  cl::desc("Compatibility option: ignored"));

static cl::opt<bool> CO7("start-group", cl::Hidden,
  cl::desc("Compatibility option: ignored"));

static cl::opt<bool> CO8("end-group", cl::Hidden,
  cl::desc("Compatibility option: ignored"));

//===----------------------------------------------------------------------===//
//===          TARGET CODE GENERATOR OPTIONS
//===----------------------------------------------------------------------===//

static cl::opt<std::string>
MArch("march", cl::desc("Architecture to generate code for:"));

static cl::opt<std::string>
MCPU("mcpu", 
     cl::desc("Target a specific cpu type (-mcpu=help for details)"),
     cl::value_desc("cpu-name"),
     cl::init(""));

static cl::list<std::string>
TargetFeatures("target-feature", cl::desc("Target specific attributes"));

static cl::list<std::string>
MAttrs("mattr", 
  cl::CommaSeparated,
  cl::desc("Target specific attributes (-mattr=help for details)"),
  cl::value_desc("a1,+a2,-a3,..."));

cl::opt<TargetMachine::CodeGenFileType>
FileType("filetype", cl::init(TargetMachine::AssemblyFile),
  cl::desc("Choose a file type (not all types are supported by all targets):"),
  cl::values(
       clEnumValN(TargetMachine::AssemblyFile,    "asm",
                  "  Emit an assembly ('.s') file"),
       clEnumValN(TargetMachine::ObjectFile,    "obj",
                  "  Emit a native object ('.o') file [experimental]"),
       clEnumValN(TargetMachine::DynamicLibrary, "dynlib",
                  "  Emit a native dynamic library ('.so') file"
                  " [experimental]"),
       clEnumValEnd));

//===----------------------------------------------------------------------===//
//===          ADVANCED OPTIONS
//===----------------------------------------------------------------------===//

static cl::opt<std::string> ConfigDir("config-dir", cl::Optional,
    cl::desc("Specify configuration directory to override defaults"),
    cl::value_desc("dir"));

static cl::opt<bool> EmitRawCode("emit-raw-code", cl::Hidden, cl::Optional,
    cl::desc("Emit raw, unoptimized code"));

static cl::opt<bool> PipeCommands("pipe", cl::Optional,
    cl::desc("Invoke sub-commands by linking input/output with pipes"));

static cl::opt<bool> KeepTemps("keep-temps", cl::Optional,
    cl::desc("Don't delete temporary files created by ellcc"));

//===----------------------------------------------------------------------===//
//===          POSITIONAL OPTIONS
//===----------------------------------------------------------------------===//

static cl::list<std::string> Files(cl::Positional, cl::ZeroOrMore,
    cl::desc("[Sources/objects/libraries]"));

// The OptimizationList is automatically populated with registered Passes by the
// PassNameParser.
//
static cl::list<const PassInfo*, bool, PassNameParser>
OptimizationList(cl::desc("Optimizations available:"));

// Other command line options...
//

static cl::opt<bool>
PrintEachXForm("p", cl::desc("Print module after each transformation"));

static cl::opt<bool>
NoOutput("disable-output",
         cl::desc("Do not write result bitcode file"), cl::Hidden);

static cl::opt<bool>
NoVerify("disable-verify", cl::desc("Do not verify result module"), cl::Hidden);

static cl::opt<bool>
Quiet("q", cl::desc("Obsolete option"), cl::Hidden);

static cl::alias
QuietA("quiet", cl::desc("Alias for -q"), cl::aliasopt(Quiet));

static cl::opt<bool>
AnalyzeOnly("analyze", cl::desc("Only perform analysis, no optimization"));

/** The input file list.
 */
struct Input {
    sys::Path name;             ///< The input's name.
    FileTypes type;             ///< The input's type.
    Module* module;             ///< The module associated with the input, if any.
    bool temp;                  ///< true if this is contained in a temporary file.
    LangOptions LO;             ///< Information about the language.
    Input() : type(NONE), module(NULL), temp(false) {}
    Input(std::string& name, FileTypes type = NONE,
          Module* module = NULL, bool temp = false)
        : name(name), type(type), module(module), temp(temp) {}
    ~Input()
    {
        if (temp && !KeepTemps) {
            if (!name.eraseFromDisk() && Verbose) {
                outs() << "  " << name.str() << " has been deleted\n";
            }
        }
    }
    void setName(sys::Path newName)
    {
        if (temp && !KeepTemps) {
            if (!name.eraseFromDisk() && Verbose) {
                outs() << "  " << name.str() << " has been deleted\n";
            }
        }
        temp = false;
        name = newName;
    }
};
typedef std::vector<Input> InputList;

/// PrintAndExit - Prints a message to standard error and exits with an error code.
///
/// Inputs:
///  Message  - The message to print to standard error.
///
static void PrintAndExit(const std::string &Message, int errcode = 1)
{
    errs() << progname << ": " << Message << "\n";
    llvm_shutdown();
    exit(errcode);
}

/// Exit - Exit with an error code.
///
static void Exit(int errcode = 1)
{
    llvm_shutdown();
    exit(errcode);
}

//===----------------------------------------------------------------------===//
// Builtin Options
//===----------------------------------------------------------------------===//

static cl::opt<bool>
TimeReport("ftime-report",
           cl::desc("Print the amount of time each "
                          "phase of compilation takes"));

static cl::opt<bool>
Freestanding("ffreestanding",
             cl::desc("Assert that the compilation takes place in a "
                            "freestanding environment"));

static cl::opt<bool>
AllowBuiltins("fbuiltin", cl::init(true),
             cl::desc("Disable implicit builtin knowledge of functions"));


static cl::opt<bool>
MathErrno("fmath-errno", cl::init(true),
          cl::desc("Require math functions to respect errno"));

//===----------------------------------------------------------------------===//
// Language Options
//===----------------------------------------------------------------------===//

static cl::list<FileTypes> Languages("x", cl::ZeroOrMore,
    cl::desc("Specify the source language for subsequent files"),
    cl::values(
        clEnumValN(C, "c", "The C language"),
        clEnumValN(I, "c-cpp-output", "Preprocessed C language"),
        clEnumValN(H, "c-header", "A C language header"),
        clEnumValN(CC, "c++", "The C++ language"),
        clEnumValN(II, "c++-cpp-output", "Preprocessed C++ language"),
        clEnumValN(HH, "c++-header", "A C++ language header"),
        clEnumValN(LL, "llvm-as", "An LLVM assembly file"),
        clEnumValN(LLX, "llvm-as-with-cpp", "An LLVM assembly file that must be preprocessed"),
        clEnumValN(BC, "llvm-bc", "An LLVM bitcode file"),
        clEnumValN(UBC, "llvm-bc", "An unoptimized LLVM bitcode file"),
        clEnumValN(S, "assembly", "An assembly file"),
        clEnumValN(SX, "assembly-with-cpp", "An assembly file that must be preprocessed"),
        clEnumValN(O, "other", "An object file, linker command file, etc"),
        clEnumValEnd));

static cl::opt<bool>
OverflowChecking("ftrapv",
                 cl::desc("Trap on integer overflow"),
                 cl::init(false));

static void InitializeCOptions(LangOptions &Options) {
    // Do nothing.
}

static void InitializeLangOptions(LangOptions &Options, FileTypes FT)
{
  // FIXME: implement -fpreprocessed mode.
  bool NoPreprocess = false;
  
  switch (FT) {
  default: assert(0 && "Unknown language kind!");
  case SX:
    Options.AsmPreprocessor = 1;
    // FALLTHROUGH
  case I:
    NoPreprocess = true;
    // FALLTHROUGH
  case C:
    InitializeCOptions(Options);
    break;
  case II:
    NoPreprocess = true;
    // FALLTHROUGH
  case CC:
    Options.CPlusPlus = 1;
    break;
  }
  
  Options.OverflowChecking = OverflowChecking;
}

enum LangStds {
  lang_unspecified,  
  lang_KandR_C,
  lang_c89, lang_c99,
  lang_gnu_START,
  lang_gnu2_KandR_C = lang_gnu_START,
  lang_gnu3_KandR_C, lang_gnu99,
  lang_cxx98, lang_gnucxx98,
  lang_cxx0x, lang_gnucxx0x
};

static const char* LangStdsNames[] =
{
    "unspecified",
    "K&R C",
    "C89",
    "C99",
    "GNU2 K&R C",
    "GNU3 K&R C",
    "GNUC99",
    "C++98",
    "GNUC++98",
    "C++0X",
    "GNUC++0X"
};

static cl::opt<LangStds>
LangStd("std", cl::desc("Language standard to compile for"),
        cl::init(lang_unspecified),
  cl::values(clEnumValN(lang_KandR_C,     "K+R",            "Kerninghan & Ritchie C"),
                   clEnumValN(lang_c89,         "c89",            "ISO C 1990"),
                   clEnumValN(lang_c89,         "c90",            "ISO C 1990"),
                   clEnumValN(lang_c89,         "iso9899:1990",   "ISO C 1990"),
                   clEnumValN(lang_c99,         "c99",            "ISO C 1999"),
                   clEnumValN(lang_c99,         "c9x",            "ISO C 1999"),
                   clEnumValN(lang_c99,         "iso9899:1999",   "ISO C 1999"),
                   clEnumValN(lang_c99,         "iso9899:199x",   "ISO C 1999"),
                   clEnumValN(lang_gnu2_KandR_C,"gnu2K+R",
                              "Kerninghan & Ritchie C with gcc2 extensions"),
                   clEnumValN(lang_gnu3_KandR_C,"gnu3K+R",
                              "Kerninghan & Ritchie C with gcc3 extensions"),
                   clEnumValN(lang_gnu99,       "gnu99",
                              "ISO C 1999 with GNU extensions (default for C)"),
                   clEnumValN(lang_gnu99,       "gnu9x",
                              "ISO C 1999 with GNU extensions"),
                   clEnumValN(lang_cxx98,       "c++98",
                              "ISO C++ 1998 with amendments"),
                   clEnumValN(lang_gnucxx98,    "gnu++98",
                              "ISO C++ 1998 with amendments and GNU "
                              "extensions (default for C++)"),
                   clEnumValN(lang_cxx0x,       "c++0x",
                              "Upcoming ISO C++ 200x with amendments"),
                   clEnumValN(lang_gnucxx0x,    "gnu++0x",
                              "Upcoming ISO C++ 200x with amendments and GNU "
                              "extensions"),
                   clEnumValEnd));

static cl::opt<bool>
NoOperatorNames("fno-operator-names",
                cl::desc("Do not treat C++ operator name keywords as "
                               "synonyms for operators"));

static cl::opt<bool>
WritableStrings("fwritable-strings",
              cl::desc("Store string literals as writable data"));

static cl::opt<bool>
NoLaxVectorConversions("fno-lax-vector-conversions",
                       cl::desc("Disallow implicit conversions between "
                                      "vectors with a different number of "
                                      "elements or different element types"));

static cl::opt<bool>
EnableBlocks("fblocks", cl::desc("enable the 'blocks' language feature"));

static cl::opt<bool>
EnableHeinousExtensions("fheinous-gnu-extensions",
   cl::desc("enable GNU extensions that you really really shouldn't use"),
                        cl::ValueDisallowed, cl::Hidden);

static cl::opt<bool>
ObjCNonFragileABI("fobjc-nonfragile-abi",
                  cl::desc("enable objective-c's nonfragile abi"));


static cl::opt<bool>
EmitAllDecls("femit-all-decls",
              cl::desc("Emit all declarations, even if unused"));

static cl::opt<bool>
Exceptions("fexceptions",
           cl::desc("Enable support for exception handling"));

static cl::opt<bool>
GNURuntime("fgnu-runtime",
            cl::desc("Generate output compatible with the standard GNU "
                           "Objective-C runtime"));

static cl::opt<bool>
NeXTRuntime("fnext-runtime",
            cl::desc("Generate output compatible with the NeXT "
                           "runtime"));



static cl::opt<bool>
Trigraphs("trigraphs", cl::desc("Process trigraph sequences"));

static cl::opt<unsigned>
TemplateDepth("ftemplate-depth", cl::init(99),
              cl::desc("Maximum depth of recursive template "
                             "instantiation"));
static cl::opt<bool>
DollarsInIdents("fdollars-in-identifiers",
                cl::desc("Allow '$' in identifiers"));


static cl::opt<bool>
OptSize("Os", cl::desc("Optimize for size"));

static cl::opt<bool>
NoCommon("fno-common",
         cl::desc("Compile common globals like normal definitions"),
         cl::ValueDisallowed);

static cl::opt<std::string>
MainFileName("main-file-name",
             cl::desc("Main file name to use for debug info"));

// FIXME: Also add an "-fno-access-control" option.
static cl::opt<bool>
AccessControl("faccess-control", 
              cl::desc("Enable C++ access control"));

static cl::opt<unsigned>
PICLevel("pic-level", cl::desc("Value for __PIC__"));

static cl::opt<bool>
StaticDefine("static-define", cl::desc("Should __STATIC__ be defined"));

static cl::opt<bool>
MSVCCompat("msvc", cl::desc("Microsoft Visual C/C++ compatability"));

static void InitializeLanguageStandard(LangOptions &LO, FileTypes FT,
                                       TargetInfo& TI,
                                       const StringMap<bool> &Features) {
  // Allow the target to set the default the langauge options as it sees fit.
  TI.getDefaultLangOptions(LO);

  // Pass the map of target features to the target for validation and
  // processing.
  TI.HandleTargetFeatures(Features);
  
  if (LangStd == lang_unspecified) {
    // Based on the base language, pick one.
    switch (FT) {
    case NONE: assert(0 && "Unknown base language");
    default:
      // Object files, etc.
      return;
    case C:
    case SX:
    case I:
      LangStd = lang_gnu99;
      break;
    case CC:
    case II:
      LangStd = lang_gnucxx98;
      break;
    }
  }
  
  if (Verbose) {
      outs() << "  language standard:" << LangStdsNames[LangStd] << "\n";
  }
  switch (LangStd) {
  default: assert(0 && "Unknown language standard!");
  case lang_gnucxx0x:
    LO.GNU_Cplusplus0x();
    break;
  case lang_cxx0x:
    LO.ANSI_Cplusplus0x();
    break;
  case lang_gnucxx98:
    LO.GNU_Cplusplus98();
    break;
  case lang_cxx98:
    LO.ANSI_Cplusplus98();
    break;
  case lang_gnu99:
    LO.GNU_C99();
    break;
  case lang_c99:
    LO.ANSI_C99();
    break;
  case lang_c89:
    LO.ANSI_C89();
    break;
  case lang_gnu3_KandR_C:
    LO.GNU3_KandR_C();
    break;
  case lang_gnu2_KandR_C:
    LO.GNU3_KandR_C();
    break;
  case lang_KandR_C:
    LO.KandR_C();
    break;
  }
  
  if (LO.CPlusPlus) {
    LO.CXXOperatorNames = !NoOperatorNames;
  }
  
  // Mimicing gcc's behavior, trigraphs are only enabled if -trigraphs
  // is specified, or -std is set to a conforming mode.
  if (Trigraphs.getPosition())
    LO.Trigraphs = Trigraphs;  // Command line option wins if specified.

  // Default to not accepting '$' in identifiers when preprocessing assembler,
  // but do accept when preprocessing C.  FIXME: these defaults are right for
  // darwin, are they right everywhere?
  if (DollarsInIdents.getPosition())  // Explicit setting overrides default.
    LO.DollarIdents = DollarsInIdents;
  
  LO.WritableStrings = WritableStrings;
  if (NoLaxVectorConversions.getPosition())
      LO.LaxVectorConversions = false;
  LO.Exceptions = Exceptions;
  if (EnableBlocks.getPosition())
    LO.Blocks = EnableBlocks;

  if (!AllowBuiltins)
    LO.NoBuiltin = true;
  if (Freestanding)
    LO.Freestanding = LO.NoBuiltin = true;
  
  if (EnableHeinousExtensions)
    LO.HeinousExtensions = true;

  if (AccessControl)
    LO.AccessControl = true;
  
  LO.MathErrno = MathErrno;

  LO.InstantiationDepth = TemplateDepth;

  if (EmitAllDecls)
    LO.EmitAllDecls = true;

  // The __OPTIMIZE_SIZE__ define is tied to -Oz, which we don't
  // support.
  LO.OptimizeSize = false;
  
  // -Os implies -O2
  if (OptSize || OptLevel) {
    LO.Optimize = true;
    LO.OptimizeSize = true;
  }

  assert(PICLevel <= 2 && "Invalid value for -pic-level");
  LO.PICLevel = PICLevel;

  LO.GNUInline = !LO.C99;
  // FIXME: This is affected by other options (-fno-inline). 
  LO.NoInline = !OptSize && !OptLevel;

  LO.Static = StaticDefine;

  if (MSVCCompat) {
      LO.MSVC_bug_compatibility();
  }

  if (MainFileName.getPosition())
    LO.setMainFileName(MainFileName.c_str());

}

//===----------------------------------------------------------------------===//
// Target Triple Processing.
//===----------------------------------------------------------------------===//

static cl::opt<std::string>
TargetTriple("triple",
  cl::desc("Specify target triple (e.g. mips-unknown-elf)"));

static cl::opt<std::string>
Arch("arch", cl::desc("Specify target architecture (e.g. nios2)"));

/// CreateTargetTriple - Process the various options that affect the target
/// triple and build a final aggregate triple that we are compiling for.
static std::string CreateTargetTriple()
{
    // Initialize base triple.  If a -triple option has been specified, use
    // that triple.  Otherwise, default to the host triple.
    std::string Triple = TargetTriple;

    if (Triple.empty()) {
        // See if a target machine is given in the program name. (<arch>-<format>)
        std::string::size_type dash;
        dash = progname.find('-');
        if (dash != std::string::npos) {
            Arch = progname.substr(0, dash);
            dash = progname.find('-', dash + 1);
            if (dash != std::string::npos) {
                Triple = progname.substr(0, dash);
            }
        } else {
            // Match the host for a simple program name, e.g. "ecc".
            Triple = sys::getHostTriple();
            dash = progname.find('-');
            if (dash != std::string::npos) {
                Arch = progname.substr(0, dash);
            }
        }
    }
  
    if (!Arch.empty()) {
        // Decompose the base triple into "arch" and suffix.
        std::string::size_type FirstDashIdx = Triple.find('-');
    
        if (FirstDashIdx == std::string::npos) {
            PrintAndExit(std::string("Malformed target triple: \"")
                         + Triple.c_str() + "\" ('-' could not be found)");
        }
    
        // Canonicalize the triple to use LLVM machine names.
        if (machines.find(Arch) == machines.end()) {
            PrintAndExit(Arch + " is not a valid machine name");
        }
    
        Triple = machines[Arch] + std::string(Triple.begin()+FirstDashIdx, Triple.end());
    }

    return Triple;
}

/** Check to see if a given file exists in the standard places.
 */
static void findFiles(std::vector<std::string>& found, std::string what, std::string where = "")
{
    if (!PrefixPath.isEmpty()) {
        
        // Stuff will be in, e.g. ${prefix}/libecc.
        sys::Path MainPath = PrefixPath;
        MainPath.appendComponent("libecc");

        // Get foo/libecc/<version>[/<where>]/<triple>/<what>
        sys::Path path = MainPath;
        path.appendComponent(ELLCC_VERSION_STRING);
        if (where.size()) {
            path.appendComponent(where);
        }
        path.appendComponent(TargetTriple);
        if (what.size()) {
            path.appendComponent(what);
        }
        if (path.exists()) {
            found.push_back(path.c_str());
            if (Verbose) {
                outs() << "  found: " << path.c_str() << "\n";
            }
        }

        if (Arch.size()) {
            // Get foo/libecc/<version>[/<where>]/<arch>/<what>
            path = MainPath;
            path.appendComponent(ELLCC_VERSION_STRING);
            if (where.size()) {
                path.appendComponent(where);
            }
            path.appendComponent(Arch);
            if (what.size()) {
                path.appendComponent(what);
            }
            if (path.exists()) {
                found.push_back(path.c_str());
                if (Verbose) {
                    outs() << "  found: " << path.c_str() << "\n";
                }
            }
        }

        // Get foo/libecc[/<where>]/<triple>/<what>
        path = MainPath;
        if (where.size()) {
            path.appendComponent(where);
        }
        path.appendComponent(TargetTriple);
        if (what.size()) {
            path.appendComponent(what);
        }
        if (path.exists()) {
            found.push_back(path.c_str());
            if (Verbose) {
                outs() << "  found: " << path.c_str() << "\n";
            }
        }

        // Get foo/libecc[/<where>]/<arch>/<what>
        path = MainPath;
        if (where.size()) {
            path.appendComponent(where);
        }
        path.appendComponent(Arch);
        if (what.size()) {
            path.appendComponent(what);
        }
        if (path.exists()) {
            found.push_back(path.c_str());
            if (Verbose) {
                outs() << "  found: " << path.c_str() << "\n";
            }
        }

        // Get foo/libecc/<version>[/<where>]/<what>
        path = MainPath;
        path.appendComponent(ELLCC_VERSION_STRING);
        if (where.size()) {
            path.appendComponent(where);
        }
        if (what.size()) {
            path.appendComponent(what);
        }
        if (path.exists()) {
            found.push_back(path.c_str());
            if (Verbose) {
                outs() << "  found: " << path.c_str() << "\n";
            }
        }

        // Get foo/libecc[/<where>]/<what>
        path = MainPath;
        if (where.size()) {
            path.appendComponent(where);
        }
        if (what.size()) {
            path.appendComponent(what);
        }
        if (path.exists()) {
            found.push_back(path.c_str());
            if (Verbose) {
                outs() << "  found: " << path.c_str() << "\n";
            }
        }
    }
}

// ---------- Define Printers for module and function passes ------------
namespace {

struct CallGraphSCCPassPrinter : public CallGraphSCCPass {
  static char ID;
  const PassInfo *PassToPrint;
  CallGraphSCCPassPrinter(const PassInfo *PI) : 
    CallGraphSCCPass((intptr_t)&ID), PassToPrint(PI) {}

  virtual bool runOnSCC(std::vector<CallGraphNode *>&SCC) {
    if (!Quiet) {
      outs() << "Printing analysis '" << PassToPrint->getPassName() << "':\n";

      for (unsigned i = 0, e = SCC.size(); i != e; ++i) {
        Function *F = SCC[i]->getFunction();
        if (F) 
          getAnalysisID<Pass>(PassToPrint).print(outs(), F->getParent());
      }
    }
    // Get and print pass...
    return false;
  }
  
  virtual const char *getPassName() const { return "'Pass' Printer"; }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequiredID(PassToPrint);
    AU.setPreservesAll();
  }
};

char CallGraphSCCPassPrinter::ID = 0;

struct ModulePassPrinter : public ModulePass {
  static char ID;
  const PassInfo *PassToPrint;
  ModulePassPrinter(const PassInfo *PI) : ModulePass((intptr_t)&ID),
                                          PassToPrint(PI) {}

  virtual bool runOnModule(Module &M) {
    if (!Quiet) {
      outs() << "Printing analysis '" << PassToPrint->getPassName() << "':\n";
      getAnalysisID<Pass>(PassToPrint).print(outs(), &M);
    }

    // Get and print pass...
    return false;
  }

  virtual const char *getPassName() const { return "'Pass' Printer"; }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequiredID(PassToPrint);
    AU.setPreservesAll();
  }
};

char ModulePassPrinter::ID = 0;
struct FunctionPassPrinter : public FunctionPass {
  const PassInfo *PassToPrint;
  static char ID;
  FunctionPassPrinter(const PassInfo *PI) : FunctionPass((intptr_t)&ID),
                                            PassToPrint(PI) {}

  virtual bool runOnFunction(Function &F) {
    if (!Quiet) { 
      outs() << "Printing analysis '" << PassToPrint->getPassName()
             << "' for function '" << F.getName() << "':\n";
    }
    // Get and print pass...
    getAnalysisID<Pass>(PassToPrint).print(outs(), F.getParent());
    return false;
  }

  virtual const char *getPassName() const { return "FunctionPass Printer"; }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequiredID(PassToPrint);
    AU.setPreservesAll();
  }
};

char FunctionPassPrinter::ID = 0;

struct LoopPassPrinter : public LoopPass {
  static char ID;
  const PassInfo *PassToPrint;
  LoopPassPrinter(const PassInfo *PI) : 
    LoopPass((intptr_t)&ID), PassToPrint(PI) {}

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) {
    if (!Quiet) {
      outs() << "Printing analysis '" << PassToPrint->getPassName() << "':\n";
      getAnalysisID<Pass>(PassToPrint).print(outs(), 
                                  L->getHeader()->getParent()->getParent());
    }
    // Get and print pass...
    return false;
  }
  
  virtual const char *getPassName() const { return "'Pass' Printer"; }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequiredID(PassToPrint);
    AU.setPreservesAll();
  }
};

char LoopPassPrinter::ID = 0;

struct BasicBlockPassPrinter : public BasicBlockPass {
  const PassInfo *PassToPrint;
  static char ID;
  BasicBlockPassPrinter(const PassInfo *PI) 
    : BasicBlockPass((intptr_t)&ID), PassToPrint(PI) {}

  virtual bool runOnBasicBlock(BasicBlock &BB) {
    if (!Quiet) {
      outs() << "Printing Analysis info for BasicBlock '" << BB.getName()
             << "': Pass " << PassToPrint->getPassName() << ":\n";
    }

    // Get and print pass...
    getAnalysisID<Pass>(PassToPrint).print(outs(), BB.getParent()->getParent());
    return false;
  }

  virtual const char *getPassName() const { return "BasicBlockPass Printer"; }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequiredID(PassToPrint);
    AU.setPreservesAll();
  }
};

char BasicBlockPassPrinter::ID = 0;

void AddStandardCompilePasses(PassManager &PM) {
  PM.add(createVerifierPass());                  // Verify that input is correct

  addOnePass(&PM, createLowerSetJmpPass(), VerifyEach);          // Lower llvm.setjmp/.longjmp

  // If the -strip-debug command line option was specified, do it.
  if (StripDebug)
    addOnePass(&PM, createStripSymbolsPass(true), VerifyEach);

  if (OptLevel == OPT_NONE) return;

  Pass *InliningPass = !DisableInline ? createFunctionInliningPass() : 0;
  createStandardModulePasses(&PM, OptLevel,
                             OptSize,
                             /*UnitAtATime=*/ true,
                             /*UnrollLoops=*/ true,
                             /*SimplifyLibCalls=*/ true,
                             /*HaveExceptions=*/ true,
                             InliningPass);
}

static TargetInfo::RaiseInstructionsList* RaiseList;
static unsigned NumRaises;
static void InitializeRaiseList(Module* M)
{
    TI->getRaiseInstructionsList(M->getContext(), RaiseList, NumRaises);
    for (unsigned index = 0; index < NumRaises; ++index) {
        // Create a reference to the function for linking.
        M->getOrInsertFunction(RaiseList[index].Name, RaiseList[index].FuncType);
    }
}

static void AddStandardLinkPasses(PassManager &PM)
{
    PM.add(createVerifierPass());               // Verify that input is correct

    // If the -strip-debug command line option was specified, do it.
    if (StripDebug)
        addOnePass(&PM, createStripSymbolsPass(true), VerifyEach);

    if (NumRaises) {
        addOnePass(&PM, createRaiseInstructionsPass(RaiseList, NumRaises), VerifyEach);
    }

    if (!DisableInternalize)
        addOnePass(&PM, createInternalizePass(exportList), VerifyEach);

    if (OptLevel <= OPT_FAST_COMPILE) {
        addOnePass(&PM, createGlobalDCEPass(), VerifyEach); // Remove dead functions.
        return;
    }

    if (!DisableInternalize)
        addOnePass(&PM, createInternalizePass(exportList), VerifyEach);

    createStandardLTOPasses(&PM, /*Internalize=*/ false,
                            /*RunInliner=*/ !DisableInline,
                            /*VerifyEach=*/ VerifyEach);
}

} // anonymous namespace

//===----------------------------------------------------------------------===//
//===          GetFileType - determine type of a file
//===----------------------------------------------------------------------===//
static const FileTypes GetFileType(const std::string& fname, unsigned pos) {
    static std::vector<FileTypes>::iterator langIt = Languages.begin();
    static FileTypes CurrLang = O;

    // If a -x LANG option has been specified ..
    if (langIt != Languages.end())
        // If the -x LANG option came before the current file on command line
        if (Languages.getPosition( langIt - Languages.begin() ) < pos) {
            // use that language
            CurrLang = *langIt++;
            return CurrLang;
        }

    // If there's a current language in effect
    if (CurrLang != O)
        return CurrLang; // use that language

    // otherwise just determine lang from the filename's suffix
    std::string ext(fname.substr(fname.rfind('.', fname.size()) + 1));
    if (extToLang.find(ext) == extToLang.end()) {
        return O;
    }

    return extToLang[ext];
}

/// Optimize - Perform link time optimizations. This will run the scalar
/// optimizations, any loaded plugin-optimization modules, and then the
/// inter-procedural optimizations if applicable.
static void Optimize(Module* M)
{
  // Instantiate the pass manager to organize the passes.
  PassManager PM;

  // Add an appropriate TargetData instance for this module...
  addOnePass(&PM, new TargetData(M), VerifyEach);

  // Add the standard passes.
  AddStandardLinkPasses(PM);

  // If the -s or -S command line options were specified, strip the symbols out
  // of the resulting program to make it smaller.  -s and -S are GNU ld options
  // that we are supporting; they alias -strip-all and -strip-debug.
  if (Strip || StripDebug)
    addOnePass(&PM, createStripSymbolsPass(StripDebug && !Strip), VerifyEach);

  // Create a new optimization pass for each one specified on the command line
  for (unsigned i = 0; i < OptimizationList.size(); ++i) {
    const PassInfo *Opt = OptimizationList[i];
    if (Opt->getNormalCtor())
      addOnePass(&PM, Opt->getNormalCtor()(), VerifyEach);
    else
      errs() << progname << ": cannot create optimization pass: " << Opt->getPassName() 
                << "\n";
  }

  // The user's passes may leave cruft around. Clean up after them them but
  // only if we haven't got optimizations enabled
  if (OptLevel != OPT_NONE) {
    addOnePass(&PM, createInstructionCombiningPass(), VerifyEach);
    addOnePass(&PM, createCFGSimplificationPass(), VerifyEach);
    addOnePass(&PM, createDeadCodeEliminationPass(), VerifyEach);
    addOnePass(&PM, createGlobalDCEPass(), VerifyEach);
  }

  // Make sure everything is still good.
  if (VerifyEach)
      PM.add(createVerifierPass());

  // Run our queue of passes all at once now, efficiently.
  PM.run(*M);
}

static void PrintCommand(const std::vector<const char*> &args) {
  std::vector<const char*>::const_iterator I = args.begin(), E = args.end(); 
  for (; I != E; ++I)
    if (*I)
      outs() << *I << " ";
  outs() << "\n";
}

//===----------------------------------------------------------------------===//
// Preprocessor Initialization
//===----------------------------------------------------------------------===//

// FIXME: Preprocessor builtins to support.
//   -A...    - Play with #assertions
//   -undef   - Undefine all predefined macros

static cl::list<std::string>
D_macros("D", cl::value_desc("macro"), cl::Prefix,
       cl::desc("Predefine the specified macro"));
static cl::list<std::string>
U_macros("U", cl::value_desc("macro"), cl::Prefix,
         cl::desc("Undefine the specified macro"));

static cl::list<std::string>
ImplicitIncludes("include", cl::value_desc("file"),
                 cl::desc("Include file before parsing"));
static cl::list<std::string>
ImplicitMacroIncludes("imacros", cl::value_desc("file"),
                      cl::desc("Include macros from file before parsing"));

static cl::opt<std::string>
ImplicitIncludePTH("include-pth", cl::value_desc("file"),
                   cl::desc("Include file before parsing"));

//===----------------------------------------------------------------------===//
// PTH.
//===----------------------------------------------------------------------===//

static cl::opt<std::string>
TokenCache("token-cache", cl::value_desc("path"),
           cl::desc("Use specified token cache file"));

//===----------------------------------------------------------------------===//
// Preprocessor include path information.
//===----------------------------------------------------------------------===//

// This tool exports a large number of command line options to control how the
// preprocessor searches for header files.  At root, however, the Preprocessor
// object takes a very simple interface: a list of directories to search for.
// 
// FIXME: -nostdinc++
// FIXME: -imultilib
//

static cl::opt<bool>
nostdinc("nostdinc", cl::desc("Disable standard #include directories"));

// Various command line options.  These four add directories to each chain.
static cl::list<std::string>
F_dirs("F", cl::value_desc("directory"), cl::Prefix,
       cl::desc("Add directory to framework include search path"));
static cl::list<std::string>
I_dirs("I", cl::value_desc("directory"), cl::Prefix,
       cl::desc("Add directory to include search path"));
static cl::list<std::string>
idirafter_dirs("idirafter", cl::value_desc("directory"), cl::Prefix,
               cl::desc("Add directory to AFTER include search path"));
static cl::list<std::string>
iquote_dirs("iquote", cl::value_desc("directory"), cl::Prefix,
               cl::desc("Add directory to QUOTE include search path"));
static cl::list<std::string>
isystem_dirs("isystem", cl::value_desc("directory"), cl::Prefix,
            cl::desc("Add directory to SYSTEM include search path"));

// These handle -iprefix/-iwithprefix/-iwithprefixbefore.
static cl::list<std::string>
iprefix_vals("iprefix", cl::value_desc("prefix"), cl::Prefix,
             cl::desc("Set the -iwithprefix/-iwithprefixbefore prefix"));
static cl::list<std::string>
iwithprefix_vals("iwithprefix", cl::value_desc("dir"), cl::Prefix,
     cl::desc("Set directory to SYSTEM include search path with prefix"));
static cl::list<std::string>
iwithprefixbefore_vals("iwithprefixbefore", cl::value_desc("dir"),
                       cl::Prefix,
            cl::desc("Set directory to include search path with prefix"));

static cl::opt<std::string>
isysroot("isysroot", cl::value_desc("dir"), cl::init("/"),
         cl::desc("Set the system root directory (usually /)"));

// Finally, implement the code that groks the options above.

/// InitializeIncludePaths - Process the -I options and set them in the
/// HeaderSearch object.
void InitializeIncludePaths(HeaderSearch &Headers,
                            FileManager &FM, const LangOptions &Lang) {
  InitHeaderSearch Init(Headers, Verbose, isysroot);

  // Handle -I... and -F... options, walking the lists in parallel.
  unsigned Iidx = 0, Fidx = 0;
  while (Iidx < I_dirs.size() && Fidx < F_dirs.size()) {
    if (I_dirs.getPosition(Iidx) < F_dirs.getPosition(Fidx)) {
      Init.AddPath(I_dirs[Iidx], InitHeaderSearch::Angled, false, true, false);
      ++Iidx;
    } else {
      Init.AddPath(F_dirs[Fidx], InitHeaderSearch::Angled, false, true, true);
      ++Fidx;
    }
  }
  
  // Consume what's left from whatever list was longer.
  for (; Iidx != I_dirs.size(); ++Iidx)
    Init.AddPath(I_dirs[Iidx], InitHeaderSearch::Angled, false, true, false);
  for (; Fidx != F_dirs.size(); ++Fidx)
    Init.AddPath(F_dirs[Fidx], InitHeaderSearch::Angled, false, true, true);
  
  // Handle -idirafter... options.
  for (unsigned i = 0, e = idirafter_dirs.size(); i != e; ++i)
    Init.AddPath(idirafter_dirs[i], InitHeaderSearch::After,
        false, true, false);
  
  // Handle -iquote... options.
  for (unsigned i = 0, e = iquote_dirs.size(); i != e; ++i)
    Init.AddPath(iquote_dirs[i], InitHeaderSearch::Quoted, false, true, false);
  
  // Handle -isystem... options.
  for (unsigned i = 0, e = isystem_dirs.size(); i != e; ++i)
    Init.AddPath(isystem_dirs[i], InitHeaderSearch::System, false, true, false);

  // Walk the -iprefix/-iwithprefix/-iwithprefixbefore argument lists in
  // parallel, processing the values in order of occurance to get the right
  // prefixes.
  {
    std::string Prefix = "";  // FIXME: this isn't the correct default prefix.
    unsigned iprefix_idx = 0;
    unsigned iwithprefix_idx = 0;
    unsigned iwithprefixbefore_idx = 0;
    bool iprefix_done           = iprefix_vals.empty();
    bool iwithprefix_done       = iwithprefix_vals.empty();
    bool iwithprefixbefore_done = iwithprefixbefore_vals.empty();
    while (!iprefix_done || !iwithprefix_done || !iwithprefixbefore_done) {
      if (!iprefix_done &&
          (iwithprefix_done || 
           iprefix_vals.getPosition(iprefix_idx) < 
           iwithprefix_vals.getPosition(iwithprefix_idx)) &&
          (iwithprefixbefore_done || 
           iprefix_vals.getPosition(iprefix_idx) < 
           iwithprefixbefore_vals.getPosition(iwithprefixbefore_idx))) {
        Prefix = iprefix_vals[iprefix_idx];
        ++iprefix_idx;
        iprefix_done = iprefix_idx == iprefix_vals.size();
      } else if (!iwithprefix_done &&
                 (iwithprefixbefore_done || 
                  iwithprefix_vals.getPosition(iwithprefix_idx) < 
                  iwithprefixbefore_vals.getPosition(iwithprefixbefore_idx))) {
        Init.AddPath(Prefix+iwithprefix_vals[iwithprefix_idx], 
                InitHeaderSearch::System, false, false, false);
        ++iwithprefix_idx;
        iwithprefix_done = iwithprefix_idx == iwithprefix_vals.size();
      } else {
        Init.AddPath(Prefix+iwithprefixbefore_vals[iwithprefixbefore_idx], 
                InitHeaderSearch::Angled, false, false, false);
        ++iwithprefixbefore_idx;
        iwithprefixbefore_done = 
          iwithprefixbefore_idx == iwithprefixbefore_vals.size();
      }
    }
  }

  Init.AddDefaultEnvVarPaths(Lang);

  // Add the ellcc headers, which are relative to the ellcc binary.
  std::vector<std::string> found;
  if (Arch.size()) {
    findFiles(found, "", "include");
  } else {
      found.push_back("/usr/include");
  }
  findFiles(found, "include", "lib");   // For stddef.h, stdarg.h, etc.

  for (size_t i = 0; i < found.size(); ++i) {
      // We pass true to ignore sysroot so that we *always* look for ecc headers
      // relative to our executable, never relative to -isysroot.
      Init.AddPath(found[i], InitHeaderSearch::System,
                   false, false, false, true /*ignore sysroot*/);
  }
  
  if (!nostdinc) 
    Init.AddDefaultSystemIncludePaths(Lang);

  // Now that we have collected all of the include paths, merge them all
  // together and tell the preprocessor about them.
  
  Init.Realize();
}

void InitializePreprocessorInitOptions(PreprocessorInitOptions &InitOpts)
{
  // Add macros from the command line.
  unsigned d = 0, D = D_macros.size();
  unsigned u = 0, U = U_macros.size();
  while (d < D || u < U) {
    if (u == U || (d < D && D_macros.getPosition(d) < U_macros.getPosition(u)))
      InitOpts.addMacroDef(D_macros[d++]);
    else
      InitOpts.addMacroUndef(U_macros[u++]);
  }

  // If -imacros are specified, include them now.  These are processed before
  // any -include directives.
  for (unsigned i = 0, e = ImplicitMacroIncludes.size(); i != e; ++i)
    InitOpts.addMacroInclude(ImplicitMacroIncludes[i]);

  if (!ImplicitIncludePTH.empty() || !ImplicitIncludes.empty()) {
    // We want to add these paths to the predefines buffer in order, make a
    // temporary vector to sort by their occurrence.
    SmallVector<std::pair<unsigned, std::string*>, 8> OrderedPaths;

    if (!ImplicitIncludePTH.empty())
      OrderedPaths.push_back(std::make_pair(ImplicitIncludePTH.getPosition(),
                                            &ImplicitIncludePTH));
    for (unsigned i = 0, e = ImplicitIncludes.size(); i != e; ++i)
      OrderedPaths.push_back(std::make_pair(ImplicitIncludes.getPosition(i),
                                            &ImplicitIncludes[i]));
    array_pod_sort(OrderedPaths.begin(), OrderedPaths.end());


    // Now that they are ordered by position, add to the predefines buffer.
    for (unsigned i = 0, e = OrderedPaths.size(); i != e; ++i) {
      std::string *Ptr = OrderedPaths[i].second;
      if (!ImplicitIncludes.empty() &&
          Ptr >= &ImplicitIncludes[0] &&
          Ptr <= &ImplicitIncludes[ImplicitIncludes.size()-1]) {
        InitOpts.addInclude(*Ptr, false);
      } else if (Ptr == &ImplicitIncludePTH) {
        InitOpts.addInclude(*Ptr, true);
      } 
    }
  }
}

//===----------------------------------------------------------------------===//
// Driver PreprocessorFactory - For lazily generating preprocessors ...
//===----------------------------------------------------------------------===//

namespace {
class VISIBILITY_HIDDEN DriverPreprocessorFactory : public PreprocessorFactory {
  Diagnostic        &Diags;
  const LangOptions &LO;
  TargetInfo        &TI;
  SourceManager     &SourceMgr;
  HeaderSearch      &HeaderInfo;
  
public:
  DriverPreprocessorFactory(Diagnostic &diags, const LangOptions &opts,
                            TargetInfo &TI, SourceManager &SM,
                            HeaderSearch &Headers)  
  : Diags(diags), LO(opts), TI(TI),
    SourceMgr(SM), HeaderInfo(Headers) {}
  
  
  virtual ~DriverPreprocessorFactory() {}
  
  virtual Preprocessor* CreatePreprocessor() {
    OwningPtr<PTHManager> PTHMgr;

    if (!TokenCache.empty() && !ImplicitIncludePTH.empty()) {
      fprintf(stderr, "error: cannot use both -token-cache and -include-pth "
                      "options\n");
      Exit(1);
    }
    
    // Use PTH?
    if (!TokenCache.empty() || !ImplicitIncludePTH.empty()) {
      const std::string& x = TokenCache.empty() ? ImplicitIncludePTH:TokenCache;
      PTHMgr.reset(PTHManager::Create(x, &Diags, 
                                      TokenCache.empty() ? Diagnostic::Error
                                                        : Diagnostic::Warning));
    }
    
    if (Diags.hasErrorOccurred())
      Exit(1);
    
    // Create the Preprocessor.
    OwningPtr<Preprocessor> PP(new Preprocessor(Diags, LO, TI,
                                                      SourceMgr, HeaderInfo,
                                                      PTHMgr.get()));
    
    // Note that this is different then passing PTHMgr to Preprocessor's ctor.
    // That argument is used as the IdentifierInfoLookup argument to
    // IdentifierTable's ctor.
    if (PTHMgr) {
      PTHMgr->setPreprocessor(PP.get());
      PP->setPTHManager(PTHMgr.take());
    }

    PreprocessorInitOptions InitOpts;
    InitializePreprocessorInitOptions(InitOpts);
    if (InitializePreprocessor(*PP, InitOpts))
      return 0;

    std::string ErrStr;
    bool DFG = CreateDependencyFileGen(PP.get(), ErrStr);
    if (!DFG && !ErrStr.empty()) {
      fprintf(stderr, "%s", ErrStr.c_str());
      return 0;
    }

    return PP.take();
  }
};
}

//===----------------------------------------------------------------------===//
// SourceManager initialization.
//===----------------------------------------------------------------------===//

static bool InitializeSourceManager(Preprocessor &PP,
                                    const std::string &InFile) {
  // Figure out where to get and map in the main file.
  SourceManager &SourceMgr = PP.getSourceManager();
  FileManager &FileMgr = PP.getFileManager();

  if (EmptyInputOnly) {
    const char *EmptyStr = "";
    MemoryBuffer *SB = 
      MemoryBuffer::getMemBuffer(EmptyStr, EmptyStr, "<empty input>");
    SourceMgr.createMainFileIDForMemBuffer(SB);
  } else if (InFile != "-") {
    const FileEntry *File = FileMgr.getFile(InFile);
    if (File) SourceMgr.createMainFileID(File, SourceLocation());
    if (SourceMgr.getMainFileID().isInvalid()) {
      PP.getDiagnostics().Report(FullSourceLoc(), diag::err_fe_error_reading) 
        << InFile.c_str();
      return true;
    }
  } else {
    MemoryBuffer *SB = MemoryBuffer::getSTDIN();

    // If stdin was empty, SB is null.  Cons up an empty memory
    // buffer now.
    if (!SB) {
      const char *EmptyStr = "";
      SB = MemoryBuffer::getMemBuffer(EmptyStr, EmptyStr, "<stdin>");
    }

    SourceMgr.createMainFileIDForMemBuffer(SB);
    if (SourceMgr.getMainFileID().isInvalid()) {
      PP.getDiagnostics().Report(FullSourceLoc(), diag::err_fe_error_reading_stdin);
      return true;
    }
  }

  return false;
}

/// Preprocess - Preprocess the given file.
///
/// Inputs:
///  InputFilename   - The name of the input source file.
///  OutputFilename  - The name of the file to generate.
///
/// Outputs:
///  None.
///
/// Returns non-zero value on error.
///
static int Preprocess(const std::string &OutputFilename, Input& input)
{
    // Process the -I options and set them in the HeaderInfo.
    HeaderSearch HeaderInfo(FileMgr);
    InitializeIncludePaths(HeaderInfo, FileMgr, input.LO);
    // Set up the preprocessor with these options.
    DriverPreprocessorFactory PPFactory(Diags, input.LO, *TI,
                                        *SourceMgr.get(), HeaderInfo);
    OwningPtr<Preprocessor> PP(PPFactory.CreatePreprocessor());

    if (!PP)
        PrintAndExit("Can't create a preprocessor");
    
    if (InitializeSourceManager(*PP.get(), input.name.str()))
        Exit(1);

    // Initialize builtin info.
    PP->getBuiltinInfo().InitializeBuiltins(PP->getIdentifierTable(),
                                            PP->getLangOptions().NoBuiltin);
    DoPrintPreprocessedInput(*PP.get(), OutputFilename);
    return Diags.getNumErrors() != 0;
}

/// Link - generates a native object file from the
/// specified bitcode file.
///
/// Inputs:
///  OutputFilename  - The name of the file to generate.
///  InputFilenames  - The name of the input files.
///  LibPaths        - The list of directories in which to find libraries.
///
/// Outputs:
///  None.
///
/// Returns non-zero value on error.
///
static int Link(const std::string& OutputFilename,
                std::vector<Input*>& InputFilenames,
                std::string& ErrMsg)
{
  // Choose the appropriate linker.
  std::string linker = progname == "e++" ? "c++" : "cc";
  bool hosted = true;
  if (Arch.size()) {
      hosted = false;
      linker = "ecc-ld";
  }
  // Determine the location of the ld program.
  // Look relative to the compiler binary.
  sys::Path ld = PrefixPath;
  ld.appendComponent("bin");
  ld.appendComponent(linker);
  if (!ld.canExecute()) {
    // Look in the path.
    ld = sys::Program::FindProgramByName(linker);
  }
  if (ld.isEmpty())
    PrintAndExit("Failed to find " + linker);

  // Mark the output files for removal if we get an interrupt.
  sys::RemoveFileOnSignal(sys::Path(OutputFilename));

  // Build the linker command line arguments.
  std::vector<std::string> args;
  args.push_back(ld.c_str());
  std::vector<std::string> found;
  if (!hosted) {
    // Add the ellcc linker script target.ld, which is relative to the ellcc binary.
    findFiles(found, "target.ld", "linker");
    if (found.size()) {
        args.push_back("-T");
        args.push_back(found[0]);
    }
    args.push_back("--build-id");
    if (Arch.size()) {
        args.push_back("-m" + emulations[Arch]);
    } 
    args.push_back("-static");
    args.push_back("--hash-style=gnu");
  }

  args.push_back("-o");
  args.push_back(OutputFilename);

  if (Native) {
    // Add in the library paths
    for (unsigned index = 0; index < LibPaths.size(); index++) {
        args.push_back("-L");
        args.push_back(LibPaths[index]);
    }
  }

  for (unsigned i = 0; i < InputFilenames.size(); ++i ) {
      if (InputFilenames[i]->type == AA) {
          if (Native) {
              args.push_back("-l");
              args.push_back(InputFilenames[i]->name.str());
          }
      } else {
          args.push_back(InputFilenames[i]->name.str());
      }
  }
            
    // Add the requested options
  for (unsigned index = 0; index < XLinker.size(); index++) {
      args.push_back(XLinker[index]);
  }

  if (!hosted) {
    // Add the ellcc library paths, which are relative to the ellcc binary.
    found.clear();
    findFiles(found, "lib");
    for (size_t i = 0; i < found.size(); ++i) {
        args.push_back(std::string("-L") + found[i]);
    }
  }

  // Now that "args" owns all the std::strings for the arguments, call the c_str
  // method to get the underlying string array.  We do this game so that the
  // std::string array is guaranteed to outlive the const char* array.
  std::vector<const char *> Args;
  for (unsigned i = 0, e = args.size(); i != e; ++i)
    Args.push_back(args[i].c_str());
  Args.push_back(0);

  if (Verbose) {
    outs() << "    ";
    PrintCommand(Args);
  }

  // Link the program.
  int R = sys::Program::ExecuteAndWait(
    ld, &Args[0], (const char**)environ, 0, 0, 0, &ErrMsg);
  return R;
}

/// Assemble - Assemble the given file.
///
/// Inputs:
///  InputFilename   - The name of the input source file.
///  OutputFilename  - The name of the file to generate.
///
/// Outputs:
///  None.
///
/// Returns non-zero value on error.
///
static int Assemble(const std::string &OutputFilename,
                      const std::string &InputFilename,
                      std::string& ErrMsg)
{
  // Choose the appropriate assembler.
  std::string assm = "as";
  if (Arch.size()) {
      assm = Arch + "-elf-as";
  }
  // Look relative to the compiler binary.
  sys::Path as = PrefixPath;
  as.appendComponent("bin");
  as.appendComponent(assm);
  if (!as.canExecute()) {
    // Look in the path.
    as = sys::Program::FindProgramByName(assm);
  }
  if (as.isEmpty())
    PrintAndExit("Failed to find " + assm);

  // Mark the output files for removal if we get an interrupt.
  sys::RemoveFileOnSignal(sys::Path(OutputFilename));

  // Run as to preprocess the file..
  std::vector<std::string> args;
  args.push_back(as.c_str());
  args.push_back("-o");
  args.push_back(OutputFilename);
  args.push_back(InputFilename);

  // HACK: Handle any ppc opcodes.
  if (Arch == "powerpc64") {
      args.push_back("-many");
      args.push_back("-a64");
  }
  
  // Now that "args" owns all the std::strings for the arguments, call the c_str
  // method to get the underlying string array.  We do this game so that the
  // std::string array is guaranteed to outlive the const char* array.
  std::vector<const char *> Args;
  for (unsigned i = 0, e = args.size(); i != e; ++i)
    Args.push_back(args[i].c_str());
  Args.push_back(0);
  if (Verbose) {
    outs() << "    ";
    PrintCommand(Args);
  }

  // Run as to assemble the file.
  int R = sys::Program::ExecuteAndWait(
    as, &Args[0], NULL, 0, 0, 0, &ErrMsg);
  return R;
}

/// LinkFile - check for a valid bitcode file.
///
/// Inputs:
///  File - The pathname of the bitcode file.
///
/// Outputs:
///  true if valid
///
///
static bool LinkFile(const sys::Path &File)
{
    // Make sure we can at least read the file
    if (!File.canRead())
        return false;

    // If its an archive, try to link it in
    std::string Magic;
    File.getMagicNumber(Magic, 64);
    switch (sys::IdentifyFileType(Magic.c_str(), 64))
    {
    default: llvm_unreachable("Bad file type identification");
    case sys::Unknown_FileType:
        return false;
    case sys::Bitcode_FileType:
    case sys::Archive_FileType:
        return true;

    case sys::ELF_Relocatable_FileType:
    case sys::ELF_SharedObject_FileType:
    case sys::Mach_O_Object_FileType:
    case sys::Mach_O_FixedVirtualMemorySharedLib_FileType:
    case sys::Mach_O_DynamicallyLinkedSharedLib_FileType:
    case sys::Mach_O_DynamicallyLinkedSharedLibStub_FileType:
    case sys::COFF_FileType:
        return false;
    }

    return false;
}

//===----------------------------------------------------------------------===//
//===          doMulti - Handle a phase acting on multiple files.
//===----------------------------------------------------------------------===//
static void doMulti(Phases phase, std::vector<Input*>& files,
                    InputList& result, TimerGroup& timerGroup,
                    LLVMContext& context,
                    FileTypes consumedType)
{
    switch (phase) {
    case BCLINKING: {
        if (TimeActions) {
            timers[phase]->startTimer();
        }

        // Generate the output name.
        sys::Path outputName;
        if (OutputFilename == "") {
             outputName = "a.bc";
        } else {
             outputName =  OutputFilename;
        }
        // Construct a Linker.
        Linker TheLinker(progname, outputName.str(), context, Verbose);

        // Add library paths to the linker
        TheLinker.addPaths(LibPaths);
        TheLinker.addSystemPaths();
        InitializeRaiseList(TheLinker.getModule());
        for (unsigned i = 0; i < files.size(); ++i) {
            if (files[i]->module) {
                // We have this module.
                std::string ErrorMessage;
                if (TheLinker.LinkInModule(files[i]->module, &ErrorMessage)) {
                    PrintAndExit(ErrorMessage);
                }
                if (Verbose) {
                    outs() << "  " << files[i]->name.str() << " was sent to the bitcode linker\n";
                }
                files[i]->module = NULL;         // The module has been consumed.
                files[i]->name.clear();
                files[i]->type = consumedType;
            } else {
                if (files[i]->type == AA) {
                    // A -l library.
                    if (!Native) {
                        if (Verbose) {
                            outs() << "  library " << files[i]->name.str()
                                << " was sent to the bitcode linker\n";
                        }
                        if (LinkAsLibrary) {
                            TheLinker.getModule()->addLibrary(files[i]->name.str());
                        } else {
                            bool isNative;  // RICH?
                            if (TheLinker.LinkInLibrary(files[i]->name.str(), isNative)) {
                                PrintAndExit(TheLinker.getLastError());
                            }
                        }
                    }
                } else if (LinkFile(files[i]->name)) {
                    bool isNative;
                    if(TheLinker.LinkInFile(files[i]->name, isNative)) {
                        Exit(1);
                    }
                    if (Verbose) {
                        outs() << "  " << files[i]->name.str() << " was sent to the bitcode linker\n";
                    }
                    if (!isNative) {
                        files[i]->type = consumedType;
                    }
                } else {
                    if (Verbose) {
                        outs() << "  " << files[i]->name.str()
                             << " is not a bitcode file and is ignored by the bitcode linker\n";
                    }
                }
            }
        }
            
        // Link all the items together
        Linker::ItemList Items;
        Linker::ItemList NativeLinkItems;
        if (TheLinker.LinkInItems(Items, NativeLinkItems) )
            PrintAndExit(TheLinker.getLastError());
        Module* module = TheLinker.releaseModule();

        // Optimize the module.
        Optimize(module);

        // Add the linked bitcode to the input list.
        std::string name(outputName.str());
        Input input(name, BC, module);
        input.temp = true;
        if (Verbose) {
            outs() << "  " << fileActions[filePhases[BC][phase].action].name
                << " " << outputName.str() << " added to the file list\n";
        }
        result.insert(result.begin(), input);

        if (TimeActions) {
	    timers[phase]->stopTimer();
        }
        break;
    }
    case LINKING: {
        if (TimeActions) {
            timers[phase]->startTimer();
        }

        std::string ErrMsg;  
        std::string outputName;
        if (OutputFilename == "") {
             outputName = "a.out";
        } else {
             outputName =  OutputFilename;
        }

        if (Link(outputName, files, ErrMsg) != 0) {
            if (ErrMsg.size()) {
                PrintAndExit(ErrMsg);
            } else {
                Exit(1);
            }
        }

        if (TimeActions) {
	    timers[phase]->stopTimer();
        }
        break;
    }
    default:
        // RICH: Illegal multi pass.
        break;
    }

    // Delete any consumed files.
    for (unsigned i = 0; i < files.size(); ++i ) {
        if (files[i]->temp) {
            if (!files[i]->name.eraseFromDisk() && Verbose) {
                outs() << "  " << files[i]->name.str() << " has been deleted\n";
                files[i]->name.clear();
            }
        }
    }
}

//===----------------------------------------------------------------------===//
//===          doSingle - Handle a phase acting on a single file.
//===----------------------------------------------------------------------===//
static FileTypes doSingle(Phases phase, Input& input, Elsa& elsa, FileTypes thisType,
                          LLVMContext& context)
{
    FileTypes nextType = filePhases[thisType][phase].type;

    switch (phase) {
    case PREPROCESSING: {              // Source language combining, filtering, substitution
        if (TimeActions) {
	    timers[phase]->startTimer();
        }

        if (Verbose) {
            // This file needs processing during this phase.
            outs() << "  " << fileActions[filePhases[thisType][phase].action].name << " " << input.name.str()
                << " to become " << fileTypes[nextType] << "\n";
        }

        if (input.name.str() != "-") {
            sys::Path to(input.name.getBasename());
            to.appendSuffix(langToExt[nextType]);
            if(Preprocess(to.str(), input) != 0) {
                Exit(1);
            }
            input.setName(to);
        } else {
            if(Preprocess(input.name.str(), input) != 0) {
                Exit(1);
            }
        }

        // Mark the file as a temporary file.
        input.temp = true;

        if (TimeActions) {
	    timers[phase]->stopTimer();
        }
        break;
    }
    case TRANSLATION: {              // Translate source -> LLVM bitcode/assembly
        // Elsa does its own timing.

        if (Verbose) {
            // This file needs processing during this phase.
            outs() << "  " << fileActions[filePhases[thisType][phase].action].name << " " << input.name.str()
                << " to become " << fileTypes[nextType] << "\n";
        }

        sys::Path to(input.name.getBasename());
        to.appendSuffix(langToExt[nextType]);
        if (filePhases[thisType][phase].action == CCOMPILE) {
            // Process the -I options and set them in the HeaderInfo.
            HeaderSearch HeaderInfo(FileMgr);
            InitializeIncludePaths(HeaderInfo, FileMgr, input.LO);
            // Set up the preprocessor with these options.
            DriverPreprocessorFactory PPFactory(Diags, input.LO, *TI,
                                                *SourceMgr.get(), HeaderInfo);
            OwningPtr<Preprocessor> PP(PPFactory.CreatePreprocessor());

            if (!PP)
                PrintAndExit("Can't create a preprocessor");
    
            if (InitializeSourceManager(*PP.get(), input.name.str()))
                Exit(1);

            // Initialize builtin info.
            PP->getBuiltinInfo().InitializeBuiltins(PP->getIdentifierTable(),
                                                    PP->getLangOptions().NoBuiltin);
 
            (*PP.get()).EnterMainSourceFile();
            int result = elsa.parse(*PP.get(),
                                    input.name.c_str(), to.c_str(),
                                    input.module, ParseOnly, *new llvm::LLVMContext, input.LO,
                                    false); // RICH: DebugOutput);
            if (result) {
                Exit(result);
            }
        } else {
            // RICH: Non C sources, e.g. .ll->.ubc
        }
        input.setName(to);
        break;
    }
    case OPTIMIZATION: {             // Optimize translation result
        if (TimeActions) {
	    timers[phase]->startTimer();
        }

        if (Verbose) {
            // This file needs processing during this phase.
            outs() << "  " << fileActions[filePhases[thisType][phase].action].name << " " << input.name.str()
                << " to become " << fileTypes[nextType] << "\n";
        }

        sys::Path to(input.name.getBasename());
        to.appendSuffix(langToExt[nextType]);
        std::string ErrorMessage;
        if (input.module == NULL) {
            // Load the input module...
            if (MemoryBuffer *Buffer = MemoryBuffer::getFileOrSTDIN(to.str(), &ErrorMessage)) {
                input.module = ParseBitcodeFile(Buffer, *new llvm::LLVMContext, &ErrorMessage);
                delete Buffer;
            }
        }

        if (input.module == NULL) {
            if (ErrorMessage.size()) {
                PrintAndExit(ErrorMessage);
            } else {
                PrintAndExit("bitcode didn't read correctly.");
            }
        }

        input.setName(to);

#if RICH
        // Figure out what stream we are supposed to write to...
        // FIXME: outs() is not binary!
        std::ostream *Out = &std::cout;  // Default to printing to stdout...
        if (OutputFilename != "-") {
            Out = new raw_fd_ostream(OutputFilename.c_str(), ErrorInfo,
                                     raw_fd_ostream::F_Binary);
            if (!ErrorInfo.empty()) {
                errs() << ErrorInfo << '\n';

                delete Out;
                return 1;
            }

            // Make sure that the Output file gets unlinked from the disk if we get a
            // SIGINT
            sys::RemoveFileOnSignal(sys::Path(OutputFilename));
        }

        // If the output is set to be emitted to standard out, and standard out is a
        // console, print out a warning message and refuse to do it.  We don't
        // impress anyone by spewing tons of binary goo to a terminal.
        if (!Force && !NoOutput && CheckBitcodeOutputToConsole(*Out,!Quiet)) {
            NoOutput = true;
        }
#endif

        // Create a PassManager to hold and optimize the collection of passes we are
        // about to build...
        //
        PassManager PM;

        // Add an appropriate TargetData instance for this module...
        PM.add(new TargetData(input.module));

        if (OptLevel > OPT_FAST_COMPILE) {
            AddStandardCompilePasses(PM);
        } else {
            // Do the minimum necessary.
            addOnePass(&PM, createStripDeadPrototypesPass(), VerifyEach); // Get rid of dead prototypes
            addOnePass(&PM, createDeadTypeEliminationPass(), VerifyEach); // Eliminate dead types
            if (StripDebug) {
                addOnePass(&PM, createStripSymbolsPass(true), VerifyEach);
            }
        }
    
        // Create a new optimization pass for each one specified on the command line
        for (unsigned i = 0; i < OptimizationList.size(); ++i) {
            const PassInfo *PassInf = OptimizationList[i];
            Pass *P = 0;
            if (PassInf->getNormalCtor())
              P = PassInf->getNormalCtor()();
            else
              PrintAndExit(std::string("cannot create pass: ") + PassInf->getPassName());
            if (P) {
                addOnePass(&PM, P, VerifyEach);
            
                if (AnalyzeOnly) {
                    if (dynamic_cast<BasicBlockPass*>(P))
                        PM.add(new BasicBlockPassPrinter(PassInf));
                    else if (dynamic_cast<LoopPass*>(P))
                        PM.add(new  LoopPassPrinter(PassInf));
                    else if (dynamic_cast<FunctionPass*>(P))
                        PM.add(new FunctionPassPrinter(PassInf));
                    else if (dynamic_cast<CallGraphSCCPass*>(P))
                        PM.add(new CallGraphSCCPassPrinter(PassInf));
                    else
                        PM.add(new ModulePassPrinter(PassInf));
                }
            }
          
            if (PrintEachXForm)
                PM.add(createPrintModulePass(&outs()));
        }
    
        // Check that the module is well formed on completion of optimization
        if (!NoVerify && !VerifyEach)
            PM.add(createVerifierPass());

#if RICH
        // Write bitcode out to disk or cout as the last step...
        if (!NoOutput && !AnalyzeOnly)
            PM.add(CreateBitcodeWriterPass(*Out));
#endif
    
        // Now that we have all of the passes ready, run them.
        PM.run(*input.module);

#if RICH
        // Delete the ofstream.
        if (Out != &std::cout) 
            delete Out;
#endif

        if (TimeActions) {
	    timers[phase]->stopTimer();
        }

        break;
    }
    case GENERATION: {               // Convert .bc to ...
        if (TimeActions) {
	        timers[phase]->startTimer();
        }

        std::string ErrorMessage;
        if (input.module == NULL) {
            // Load the input module...
            if (MemoryBuffer *Buffer = MemoryBuffer::getFileOrSTDIN(input.name.str(), &ErrorMessage)) {
                input.module = ParseBitcodeFile(Buffer, context, &ErrorMessage);
                delete Buffer;
            }
        }

        if (input.module == NULL) {
            // Assume we have an object file.
            nextType = O;
            break;
        }

        // Allocate target machine.  First, check whether the user has
        // explicitly specified an architecture to compile for.
        const Target *TheTarget = 0;
        if (!MArch.empty()) {
            for (TargetRegistry::iterator it = TargetRegistry::begin(),
                ie = TargetRegistry::end(); it != ie; ++it) {
                if (MArch == it->getName()) {
                    TheTarget = &*it;
                    break;
                }
            }

            if (!TheTarget) {
                errs() << progname << ": error: invalid target '" << MArch << "'.\n";
                Exit(1);
            }        
        } else {
            std::string Err;
            TheTarget = TargetRegistry::lookupTarget(input.module->getTargetTriple(),
                                                     Err);
            if (TheTarget == 0) {
                errs() << progname << ": error auto-selecting target for module '"
                       << Err << "'.  Please use the -march option to explicitly "
                       << "pick a target.\n";
                Exit(1);
            }
        }

        // Package up features to be passed to target/subtarget
        std::string FeaturesStr;
        if (MCPU.size() || MAttrs.size()) {
            SubtargetFeatures Features;
            Features.setCPU(MCPU);
            for (unsigned i = 0; i != MAttrs.size(); ++i)
                Features.AddFeature(MAttrs[i]);
            FeaturesStr = Features.getString();
        }

        std::auto_ptr<TargetMachine> target(TheTarget->createTargetMachine(input.module->getTargetTriple(),
                                                                           FeaturesStr));
        assert(target.get() && "Could not allocate target machine!");
        TargetMachine &Target = *target.get();

        sys::Path to;
        if (phase == FinalPhase && OutputFilename != "") {
            to = sys::Path(OutputFilename);
        } else {
            to = sys::Path(input.name.getBasename());
        }
#if RICH
        // Figure out where we are going to send the output...
        std::ostream *Out = NULL;
        if (phase == FinalPhase && OutputFilename != "") {
            if (OutputFilename == "-")
                return &std::cout;

            // Specified an output filename?
            // Make sure that the Out file gets unlinked from the disk if we get a
            // SIGINT
            sys::RemoveFileOnSignal(sys::Path(OutputFilename));

            return new std::ofstream(OutputFilename.c_str());
        }

        if (InputFilename == "-") {
            OutputFilename = "-";
            return &std::cout;
        }

        OutputFilename = GetFileNameRoot(InputFilename);
#endif

        bool isBinary = true;
        switch (FileType) {
        case TargetMachine::AssemblyFile:
            if (MArch[0] != 'c' || MArch[1] != 0) { // not CBE
                if (to.getSuffix() == "") {
                    to.appendSuffix("s");
                }
                isBinary = false;
                nextType = S;
            } else {
                if (to.getSuffix() == "") {
                    to.appendSuffix("cbe.c");
                }
                isBinary = false;
                nextType = CBE;
            }
            break;
        case TargetMachine::ObjectFile:
            if (to.getSuffix() == "") {
                to.appendSuffix("o");
            }
            nextType = O;
            break;
        case TargetMachine::DynamicLibrary:
            if (to.getSuffix() == "") {
                to.appendSuffix("dll");
            }
            nextType = DLL;
            break;
        }

        if (phase != FinalPhase) {
            std::string ErrMsg;
            if(to.createTemporaryFileOnDisk(false, &ErrMsg)) {
                PrintAndExit(ErrMsg);
            }
        }

        if (Verbose) {
            // This file needs processing during this phase.
            outs() << "  " << fileActions[filePhases[thisType][phase].action].name << " " << input.name.str()
                << " to become " << to.c_str() << ", " << fileTypes[nextType] << "\n";
        }

        // Make sure that the Out file gets unlinked from the disk if we get a
        // SIGINT
        sys::RemoveFileOnSignal(to);

        std::string error;
        raw_ostream *os = new raw_fd_ostream(to.c_str(), error,
                                             isBinary ? raw_fd_ostream::F_Binary
                                                      : 0);
        if (!error.empty()) {
          errs() << error << '\n';
          delete os;
          Exit(1);
        }
        formatted_raw_ostream *Out = new formatted_raw_ostream(*os,
            formatted_raw_ostream::DELETE_STREAM);

        // If this target requires addPassesToEmitWholeFile, do it now.  This is
        // used by strange things like the C backend.
        if (Target.WantsWholeFile()) {
            PassManager PM;
            PM.add(new TargetData(*Target.getTargetData()));
            if (!NoVerify)
                PM.add(createVerifierPass());

            // Ask the target to add backend passes as necessary.
            if (Target.addPassesToEmitWholeFile(PM, *Out, FileType, getCodeGenOpt())) {
                // RICH:
                errs() << progname << ": target does not support generation of this"
                    << " file type!\n";
                if (Out != &fouts()) delete Out;
                // And the Out file is empty and useless, so remove it now.
                sys::Path(OutputFilename).eraseFromDisk();
                Exit(1);
            }
            PM.run(*input.module);
        } else {
            // Build up all of the passes that we want to do to the module.
            FunctionPassManager PM(new ExistingModuleProvider(input.module));
            PM.add(new TargetData(*Target.getTargetData()));

#ifndef NDEBUG
            if (!NoVerify)
                PM.add(createVerifierPass());
#endif

            // Ask the target to add backend passes as necessary.
            ObjectCodeEmitter *OCE = 0;

            switch (Target.addPassesToEmitFile(PM, *Out, FileType, getCodeGenOpt())) {
                default:
                    assert(0 && "Invalid file model!");
                    Exit(1);
                    break;
                case FileModel::Error:
                    errs() << progname << ": target does not support generation of this"
                        << " file type!\n";
                    if (Out != &fouts()) delete Out;
                    // And the Out file is empty and useless, so remove it now.
                    sys::Path(OutputFilename).eraseFromDisk();
                    Exit(1);
                    break;
                case FileModel::AsmFile:
                    break;
                case FileModel::MachOFile:
                    OCE = AddMachOWriter(PM, *Out, Target);
                    break;
                case FileModel::ElfFile:
                    OCE = AddELFWriter(PM, *Out, Target);
                    break;
            }

            if (Target.addPassesToEmitFileFinish(PM, OCE, getCodeGenOpt())) {
                errs() << progname << ": target does not support generation of this"
                    << " file type!\n";
                if (Out != &fouts()) delete Out;
                // And the Out file is empty and useless, so remove it now.
                sys::Path(OutputFilename).eraseFromDisk();
                Exit(1);
            }

            PM.doInitialization();

            // Run our queue of passes all at once now, efficiently.
            // TODO: this could lazily stream functions out of the module.
            for (Module::iterator I = input.module->begin(), E = input.module->end(); I != E; ++I) {
                if (!I->isDeclaration()) {
                    PM.run(*I);
                }
            }

            PM.doFinalization();
        }

        Out->flush();

        // Delete the ostream if it's not a stdout stream
        if (Out != &fouts()) delete Out;

        input.setName(to);
        // Mark the file as a temporary file.
        input.temp = true;
        delete input.module;
        input.module = NULL;

        if (TimeActions) {
	    timers[phase]->stopTimer();
        }

        break;
    }
    case ASSEMBLY: {                 // Convert .s to .o
        if (TimeActions) {
	    timers[phase]->startTimer();
        }

        if (Verbose) {
            // This file needs processing during this phase.
            outs() << "  " << fileActions[filePhases[thisType][phase].action].name << " " << input.name.str()
                << " to become " << fileTypes[nextType] << "\n";
        }

        sys::Path to;
        if (phase == FinalPhase && OutputFilename != "") {
            to = sys::Path(OutputFilename);
        } else {
            to = sys::Path(input.name.getBasename());
            to.appendSuffix(langToExt[nextType]);
        }


        std::string ErrMsg;
        if (phase != FinalPhase) {
            if(to.createTemporaryFileOnDisk(false, &ErrMsg)) {
                PrintAndExit(ErrMsg);
            }
        }

        if(Assemble(to.str(), input.name.str(), ErrMsg) != 0) {
            PrintAndExit(ErrMsg);
        }

        input.setName(to);
        // Mark the file as a temporary file.
        input.temp = true;

        if (TimeActions) {
	    timers[phase]->stopTimer();
        }

        break;
    }
    default:
        // RICH: Illegal single pass.
        break;
    }
    
    return nextType;
}

/// ComputeTargetFeatures - Recompute the target feature list to only
/// be the list of things that are enabled, based on the target cpu
/// and feature list.
static void ComputeFeatureMap(TargetInfo& TI, StringMap<bool> &Features)
{
    assert(Features.empty() && "invalid map"); 

    // Initialize the feature map based on the target.
    TI.getDefaultFeatures(MCPU, Features);

    // Apply the user specified deltas.
    for (cl::list<std::string>::iterator it = TargetFeatures.begin(), 
         ie = TargetFeatures.end(); it != ie; ++it) {
        const char *Name = it->c_str();
    
        // FIXME: Don't handle errors like this.
        if (Name[0] != '-' && Name[0] != '+') {
            PrintAndExit(std::string("error: ") + progname + ": invalid target feature string: " + Name);
        }
        if (!TI.setFeatureEnabled(Features, Name + 1, (Name[0] == '+'))) {
            fprintf(stderr, "error: clang-cc: invalid target feature name: %s\n", 
                    Name + 1);
            Exit(1);
        }
  }
}

// Catch LLVM errors.
static void handleLLVMErrors(void* user_data, const std::string& reason)
{
    Diags.Report(FullSourceLoc(), diag::err_fe_internal_error) << reason;
    llvm_shutdown();
    Exit(1);
}

//===----------------------------------------------------------------------===//
// main for ellcc
//
int main(int argc, char **argv)
{
    llvm_shutdown_obj X;        // Call llvm_shutdown() on exit.
    InputList InpList;
    int status = 0;

    InitializeAllTargets();
    InitializeAllAsmPrinters();
    // Create the diagnostic client for reporting errors or for
    // implementing -verify.
    OwningPtr<DiagnosticClient> DiagClient;
    llvm_install_error_handler(handleLLVMErrors, /* user_data */ NULL);

    try {
        // Initial global variable above for convenience printing of program name.
        argv0 = argv[0];            // Save the name for later.
        progname = sys::Path(argv0).getBasename();
        PrefixPath = sys::Path::GetMainExecutable(argv0, (void*)(intptr_t)main);
        PrefixPath.eraseComponent();  // Remove prog   from foo/bin/prog
        PrefixPath.eraseComponent();  // Remove /bin   from foo/bin
        setupMappings();

        TimerGroup timerGroup("... Ellcc action timing report ...");
        for (int i = 0; i < NUM_PHASES; ++i) {
            timers[i] = new Timer(phases[i].name, timerGroup);
        }

        // RICH: Temporary work around.
        LLVMContext context;
        
        // Parse the command line options.
        cl::ParseCommandLineOptions(argc, argv, "C/C++ compiler\n");
        
        exportList.push_back("_start");         // The initial entry point.
        exportList.push_back("_estart");        // The program entry point.
        exportList.push_back("main");           // Needed for debugging (break @ main).

        if (FinalPhase == OPTIMIZATION && Native)
            FinalPhase = ASSEMBLY;

        sys::PrintStackTraceOnErrorSignal();
        PrettyStackTraceProgram X(argc, argv);

        if (FinalPhase == PREPROCESSING) {
            // Use only the preprocessor.
            filePhases[CC][PREPROCESSING].type = II;
            filePhases[CC][PREPROCESSING].action = PREPROCESS;
            filePhases[C][PREPROCESSING].type = I;
            filePhases[C][PREPROCESSING].action = PREPROCESS;
        }

        

        // If -fmessage-length=N was not specified, determine whether this
        // is a terminal and, if so, implicitly define -fmessage-length
        // appropriately.
        if (MessageLength.getNumOccurrences() == 0)
            MessageLength.setValue(sys::Process::StandardErrColumns());

        if (!NoColorDiagnostic) {
            NoColorDiagnostic.setValue(!sys::Process::StandardErrHasColors());
        }

        DiagClient.reset(new TextDiagnosticPrinter(outs(),
                                                   !NoShowColumn,
                                                   !NoCaretDiagnostics,
                                                   !NoShowLocation,
                                                   PrintSourceRangeInfo,
                                                   PrintDiagnosticOption,
                                                   !NoDiagnosticsFixIt,
                                                   MessageLength,
                                                   !NoColorDiagnostic));

        // Configure our handling of diagnostics.
        Diags.setClient(DiagClient.get());
        if (ProcessWarningOptions(Diags))
            Exit(1);

        // -I- is a deprecated GCC feature, scan for it and reject it.
        for (unsigned i = 0, e = I_dirs.size(); i != e; ++i) {
            if (I_dirs[i] == "-") {
                Diags.Report(FullSourceLoc(), diag::err_pp_I_dash_not_supported);
                I_dirs.erase(I_dirs.begin()+i);
                --i;
            }
        }

        // Get information about the target being compiled for.
        TargetTriple = CreateTargetTriple();
        TI.reset(TargetInfo::CreateTargetInfo(TargetTriple));
        if (TI == 0) {
            Diags.Report(FullSourceLoc(), diag::err_fe_unknown_triple) << TargetTriple.c_str();
            Exit(1);
        }
  
        // Compute the feature set, unfortunately this effects the language!
        StringMap<bool> Features;
        ComputeFeatureMap(*TI.get(), Features);

        // Initialize Elsa.
        Elsa elsa(timerGroup);       // Get the parsing environment.
        elsa.setup(TimeActions);
        elsa.wantBpprint = ElsaPrettyPrint;
        elsa.wantBpprintAfterElab = ElsaPrettyPrintAfterElab;
        std::vector<std::string>::iterator traceIt = ElsaTraceOpts.begin();
        for ( ; traceIt != ElsaTraceOpts.end(); ++traceIt) {
            elsa.addTrace((*traceIt).c_str());
        }

        if (Arch.size()) {
            if (FinalPhase >= BCLINKING && !NoLink && !NoCrt0) {
                // Add the ellcc crt0.o, which is relative to the ellcc binary.
                std::vector<std::string> found;
                findFiles(found, "crt0.o", "lib");
                if (found.size()) {
                    Input input(found[0], O);
                    InpList.push_back(input);
                }
            }
        }
        
        // Gather the input files and determine their types.
        std::vector<std::string>::iterator fileIt = Files.begin();
        std::vector<std::string>::iterator libIt  = Libraries.begin();
        unsigned libPos = 0, filePos = 0;
        for ( ;; )  {
            if (libIt != Libraries.end())
                libPos = Libraries.getPosition( libIt - Libraries.begin() );
            else
                libPos = 0;
            if (fileIt != Files.end())
                filePos = Files.getPosition(fileIt - Files.begin());
            else
                filePos = 0;

            if (filePos != 0 && (libPos == 0 || filePos < libPos)) {
                // Add a source file
                FileTypes type = GetFileType(*fileIt, filePos);
                if (Verbose) {
                    outs() << "  adding " << *fileIt << " as " << fileTypes[type] << "\n";
                }
                Input input(*fileIt, type);
    
                /// Create a SourceManager object.  This tracks and owns all the file
                /// buffers allocated to a translation unit.
                if (!SourceMgr)
                    SourceMgr.reset(new SourceManager());
                else
                    SourceMgr->clearIDTables();

                // Initialize language options, inferring file types from input filenames.
                DiagClient->setLangOptions(&input.LO);
                InitializeLangOptions(input.LO, type);
                InitializeLanguageStandard(input.LO, type, *TI.get(), Features);
                InpList.push_back(input);
                ++fileIt;
            } else if ( libPos != 0 && (filePos == 0 || libPos < filePos) ) {
                // Add a library
                if (Verbose) {
                    outs() << "  adding " << *libIt << " as an input library\n";
                }
                Input input(*libIt, AA);
                InpList.push_back(input);
                ++libIt;
            }
            else
                break; // we're done with the list
        }

        if (Files.empty()) {
            // No input files present.
            Files.push_back("-");
            FileTypes type = CC;
            std::string name("-");
            if (Verbose) {
                outs() << "  adding <stdin> as " << fileTypes[type] << "\n";
            }
            Input input(name, type);
            
            /// Create a SourceManager object.  This tracks and owns all the file
            /// buffers allocated to a translation unit.
            if (!SourceMgr)
                SourceMgr.reset(new SourceManager());
            else
                SourceMgr->clearIDTables();

            // Initialize language options, inferring file types from input filenames.
            DiagClient->setLangOptions(&input.LO);
            InitializeLangOptions(input.LO, type);
            InitializeLanguageStandard(input.LO, type, *TI.get(), Features);
            InpList.push_back(input);
        }

        if (Arch.size()) {
            if (FinalPhase >= BCLINKING && !NoLink) {
                // Add the ellcc libecc.a, which is relative to the ellcc binary.
                std::vector<std::string> found;
                findFiles(found, "libecc.a", "lib");
                if (found.size()) {
                    Input input(found[0], A);
                    InpList.push_back(input);
                }
                found.clear();
                findFiles(found, "libtarget.a", "lib");
                if (found.size()) {
                    Input input(found[0], A);
                    InpList.push_back(input);
                }
                found.clear();
                findFiles(found, "libcompiler-rt.a", "lib");
                if (found.size()) {
                    Input input(found[0], A);
                    InpList.push_back(input);
                }
            }
        }

        if (   OutputFilename != ""
            && InpList.size() > 1
            && (   FinalPhase != GENERATION
                && FinalPhase != ASSEMBLY
                && FinalPhase != BCLINKING
                && FinalPhase != LINKING)) {
                PrintAndExit("an output file name was specified with multiple input files");
        }

        // Go through the phases.
        InputList::iterator it;
        Phases phase;
        for(phase = PREPROCESSING; phase != NUM_PHASES; phase = (Phases)(phase + 1)) {
            if (Verbose) {
                outs() << "Phase: " << phases[phase].name << "\n";
            }

            if (phases[phase].result != NONE) {
                // This phase deals with muiltple files.
                if (NoLink) {
                    if(phase == BCLINKING || phase == LINKING)
                    // We want individual files.
                    if (Verbose) {
                        outs() << "Phase: " << phases[phase].name << " has been supressed\n";
                    }
                    continue;
                }

                std::vector<Input*> files;
                FileTypes nextType = NONE;
                for (it = InpList.begin(); it != InpList.end(); ++it) {
                    if (it->name.isEmpty()) {
                        continue;
                    }
                    nextType = filePhases[it->type][phase].type;
                    if (nextType != NONE) {
                        if (Verbose) {
                            // This file needs processing during this phase.
                            outs() << "  " << fileActions[filePhases[it->type][phase].action].name
                                << " " << it->name.str() << " to become " << fileTypes[nextType] << "\n";
                        }
                        
                        files.push_back(&*it);
                    } else {
                        if (Verbose) {
                            outs() << "  " << it->name.str() << " is ignored during this phase\n";
                        }
                    }
                }

                if (files.size()) {
                    // Perform the phase on the files.
                    doMulti(phase, files, InpList, timerGroup, context, nextType);
                }
            } else {
                for (it = InpList.begin(); it != InpList.end(); ++it) {
                    if (it->name.isEmpty()) {
                        continue;
                    }
                    if (filePhases[it->type][phase].type != NONE) {
                        // Perform the phase on the file.
                        it->type = doSingle(phase, *it, elsa, it->type, context);
                    } else {
                        if (Verbose) {
                            outs() << "  " << it->name.str() << " is ignored during this phase\n";
                        }
                    }
                }
            }

            if (phase == TRANSLATION && ParseOnly) {
                // Done parsing. Stop.
                break;
            }
            if (FinalPhase == phase) {
                for (it = InpList.begin(); it != InpList.end(); ++it) {
                    it->temp = false;
                }
            }
            if (FinalPhase == phase || Diags.hasErrorOccurred()) {
                break;
            }

        }

        // Check to see if any module files should be generated
        for (InputList::iterator it = InpList.begin(); it != InpList.end(); ++it) {
            if (it->module && !it->temp) {
                // Output the module.
                // RICH: .ll vs. .bc.
                const char* to;
                if (phase == FinalPhase && OutputFilename != "") {
                    // Override the generated name.
                    to = sys::Path(OutputFilename).c_str();
                } else {
                    to = it->name.c_str();
                }
                PassManager PM;
                std::string ErrorInfo;
                raw_ostream* out = new raw_fd_ostream(to, ErrorInfo, raw_fd_ostream::F_Binary);
                if (!ErrorInfo.empty()) {
                    errs() << progname << ": can't open " << to << " for writing\n";
                    delete out;
                    Exit(1);
                }
                if (Verbose) {
                    const char* ftype = (FinalPhase != phase) ? "temporary " : "";
                    outs() << "  creating " << ftype << "file " << to << "\n";
                }
#if RICH
                OStream L(*out);
                PM.add(new PrintModulePass(&L));
#endif
                PM.add(createBitcodeWriterPass(*out));
                PM.run(*it->module);
                delete out;
                delete it->module;
            }
        }

        for (int i = 0; i < NUM_PHASES; ++i) {
            delete timers[i];
        }
            
    } catch (const std::string& msg) {
        Diags.Report(FullSourceLoc(), diag::err_fe_internal_error) << msg;
        status =  1;
    } catch (const char* msg) {
        Diags.Report(FullSourceLoc(), diag::err_fe_internal_error) << msg;
        status =  1;
    } catch (...) {
        Diags.Report(FullSourceLoc(), diag::err_fe_internal_error)
            << "unexpected unknown exception occurred.";
        status =  1;
    }

    status =  Diags.hasErrorOccurred() ? 4 : status;
    llvm_shutdown();
    return status;
}
