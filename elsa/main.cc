// main.cc            see license.txt for copyright and terms of use
// entry-point module for a program that parses C++

#include <iostream>       // cout
#include <stdlib.h>       // exit, getenv, abort
#include <fstream>        // ofstream

#include "trace.h"        // traceAddSys
#include "syserr.h"       // xsyserror
#include "parssppt.h"     // ParseTreeAndTokens, treeMain
#include "srcloc.h"       // SourceLocManager
#include "ckheap.h"       // malloc_stats
#include "cc_env.h"       // Env
#include "cc_ast.h"       // C++ AST (r)
#include "cc_ast_aux.h"   // class LoweredASTVisitor
#include "parsetables.h"  // ParseTables
#include "cc_print.h"     // PrintEnv
#include "cc.gr.gen.h"    // CCParse
#include "nonport.h"      // getMilliseconds
#include "ptreenode.h"    // PTreeNode
#include "ptreeact.h"     // ParseTreeLexer, ParseTreeActions
#include "sprint.h"       // structurePrint
#include "strtokp.h"      // StrtokParse
#include "smregexp.h"     // regexpMatch
#include "cc_elaborate.h" // ElabVisitor
#include "integrity.h"    // IntegrityVisitor
#include "xml_file_writer.h" // XmlFileWriter
#include "xml_reader.h"   // xmlDanglingPointersAllowed
#include "xml_do_read.h"  // xmlDoRead()
#include "xml_type_writer.h" // XmlTypeWriter
#include "bpprint.h"      // bppTranslationUnit
#include "cc2c.h"         // cc_to_c
#include "LangOptions.h"
#include "TargetInfo.h"
#include "llvm/System/Host.h"

#define xstr(x) #x
#define str(x) xstr(x)

#define ELLCC 0
#define ELLCC_MINOR 1
#define ELLCC_PATCHLEVEL 0
#define ELLCC_VERSION_STRING str(ELLCC) "." str(ELLCC_MINOR) "." str(ELLCC_PATCHLEVEL)
#define ELLCC_VERSION_MODIFIER "ALPHA"
#define ELLCC_VERSION ELLCC_VERSION_STRING " " ELLCC_VERSION_MODIFIER " " __DATE__

using namespace sm;
using namespace llvm;

#include "ElsaDiagnostic.h"
#include "LexDiagnostic.h"
#include "HeaderSearch.h"
#include "InitHeaderSearch.h"
#include "Preprocessor.h"
#include "InitPreprocessor.h"
#include "FileManager.h"
#include "TextDiagnosticPrinter.h"
#include "LangOptions.h"
#include "TargetInfo.h"
#include "cc2llvm.h"      // cc_to_llvm

// LLVM
#include <llvm/Module.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/Assembly/PrintModulePass.h>
#include "llvm/Module.h"
#include "llvm/ModuleProvider.h"
#include "llvm/PassManager.h"
#include "llvm/CallGraphSCCPass.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Target/SubtargetFeature.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetMachineRegistry.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
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
#include "llvm/Support/Timer.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/LinkAllVMCore.h"
#include "llvm/Linker.h"

// true to print the tchecked C++ syntax using bpprint after
// tcheck
static bool wantBpprint = false;

// same, but after elaboration
static bool wantBpprintAfterElab = false;


// nonempty if we want to run cc2c; value of "-" means stdout
static sm::string cc2cOutputFname;

#ifdef LLVM_EXTENSION
// nonempty if we want to run cc2llvm; value of "-" means stdout
static sm::string cc2llvmOutputFname;
#endif

static const char* progname;

/// PrintAndExit - Prints a message to standard error and exits with an error code.
///
/// Inputs:
///  Message  - The message to print to standard error.
///
static void PrintAndExit(const std::string &Message, int errcode = 1)
{
    cerr << progname << ": " << Message << "\n";
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


// little check: is it true that only global declarators
// ever have Declarator::type != Declarator::var->type?
// .. no, because it's true for class members too ..
// .. it's also true of arrays declared [] but then inited ..
// .. finally, it's true of parameters whose types get
//    normalized as per cppstd 8.3.5 para 3; I didn't include
//    that case below because there's no easy way to test for it ..
// Intended to be used with LoweredASTVisitor
class DeclTypeChecker : private ASTVisitor {
public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  int instances;

public:
  DeclTypeChecker()
    : loweredVisitor(this)
    , instances(0)
  {}
  virtual ~DeclTypeChecker() {}

  virtual bool visitDeclarator(Declarator *obj);
};

bool DeclTypeChecker::visitDeclarator(Declarator *obj)
{
  if (obj->type != obj->var->type &&
      !(obj->var->flags & (DF_GLOBAL | DF_MEMBER)) &&
      !obj->type->isArrayType()) {
    instances++;
    std::cout << toString(obj->var->loc) << ": " << obj->var->name
         << " has type != var->type, but is not global or member or array\n";
  }
  return true;
}


// this scans the AST for E_variables, and writes down the locations
// to which they resolved; it's helpful for writing tests of name and
// overload resolution
class NameChecker : public ASTVisitor {
public:
  // accumulates the results
  stringBuilder sb;

public:
  NameChecker() {}

  virtual bool visitExpression(Expression *obj)
  {
    Variable *v = NULL;
    if (obj->isE_variable()) {
      v = obj->asE_variable()->var;
    }
    else if (obj->isE_fieldAcc()) {
      v = obj->asE_fieldAcc()->field;
    }

    // this output format is designed to minimize the effect of
    // changes to unrelated details
    if (v
        && !streq("__testOverload", v->name)
        && !streq("dummy",          v->name)
        && !streq("__other",        v->name) // "__other": for inserted elaboration code
        && !streq("this",           v->name) // dsw: not sure why "this" is showing up
        && !streq("operator=",      v->name) // an implicitly defined member of every class
        && v->name[0]!='~'                   // don't print dtors
        ) {
      sb << " " << v->name << "=" << sourceLocManager->getLine(v->loc);
    }

    return true;
  }
};


void if_malloc_stats()
{
  if (tracingSys("malloc_stats")) {
    malloc_stats();
  }
}


class SectionTimer {
  long start;
  long &elapsed;

public:
  SectionTimer(long &e)
    : start(getMilliseconds()),
      elapsed(e)
  {}
  ~SectionTimer()
  {
    elapsed += getMilliseconds() - start;
  }
};


void handle_xBase(Env &env, xBase &x)
{
  // typically an assertion failure from the tchecker; catch it here
  // so we can print the errors, and something about the location
  env.errors.print(std::cout);
  std::cout << x << std::endl;
  std::cout << "Failure probably related to code near " << env.locStr() << std::endl;

  // print all the locations on the scope stack; this is sometimes
  // useful when the env.locStr refers to some template code that
  // was instantiated from somewhere else
  //
  // (unfortunately, env.instantiationLocStack isn't an option b/c
  // it will have been cleared by the automatic invocation of
  // destructors unwinding the stack...)
  std::cout << "current location stack:\n";
  std::cout << env.locationStackString();

  // I changed from using exit(4) here to using abort() because
  // that way the multitest.pl script can distinguish them; the
  // former is reserved for orderly exits, whereas signals (like
  // SIGABRT) mean that something went really wrong
  abort();
}


// this is a dumb way to organize argument processing...
char *myProcessArgs(int argc, char **argv, char const *additionalInfo)
{
  // remember program name
  char const *progName = argv[0];

  #define SHIFT argv++; argc-- /* user ; */

  // process args
  while (argc >= 2) {
    if (traceProcessArg(argc, argv)) {
      continue;
    }
    else if (0==strcmp(argv[1], "-xc")) {
      // treat this as a synonym for "-tr c_lang"
      traceAddSys("c_lang");
      SHIFT;
    }
    else if (0==strcmp(argv[1], "-w")) {
      // synonym for -tr nowarnings
      traceAddSys("nowarnings");
      SHIFT;
    }
    else if (0==strcmp(argv[1], "-bpprint")) {
      wantBpprint = true;
      SHIFT;
    }
    else if (0==strcmp(argv[1], "-bpprintAfterElab")) {
      wantBpprintAfterElab = true;
      SHIFT;
    }
    else if (0==strcmp(argv[1], "-cc2c")) {
      if (argc < 3) {
        std::cout << "-cc2c requires a file name argument\n";
        exit(2);
      }
      cc2cOutputFname = argv[2];
      SHIFT;
      SHIFT;
    }
#ifdef LLVM_EXTENSION
    else if (0==strcmp(argv[1], "-cc2llvm")) {
      if (argc < 3) {
        std::cout << "-cc2llvm requires a file name argument\n";
        exit(2);
      }
      cc2llvmOutputFname = argv[2];
      SHIFT;
      SHIFT;
    }
#endif
    else {
      break;     // didn't find any more options
    }
  }

  #undef SHIFT

  if (argc != 2) {
    std::cout << "usage: " << progName << " [options] input-file\n"
            "  options:\n"
            "    -tr <flags>:       turn on given tracing flags (comma-separated)\n"
            "    -bbprint:          print parsed C++ back out using bpprint\n"
            "    -bbprintAfterElab: bpprint after elaboration\n"
            "    -cc2c <fname>:     generate C, write to <fname>; \"-\" means stdout\n"
#ifdef LLVM_EXTENSION
            "    -cc2llvm <fname>:  generate LLVM code, write to <fname>; \"-\" means stdout\n"
#endif
         << (additionalInfo? additionalInfo : "");
    exit(argc==1? 0 : 2);    // error if any args supplied
  }

  // some -tr equivalents, which are convenient because they
  // can be passed in the TRACE environment variable
  wantBpprint = wantBpprint || tracingSys("bpprint");

  return argv[1];
}

//===----------------------------------------------------------------------===//
// Preprocessor Initialization
//===----------------------------------------------------------------------===//

// FIXME: Preprocessor builtins to support.
//   -A...    - Play with #assertions
//   -undef   - Undefine all predefined macros

static llvm::cl::list<std::string>
D_macros("D", llvm::cl::value_desc("macro"), llvm::cl::Prefix,
       llvm::cl::desc("Predefine the specified macro"));
static llvm::cl::list<std::string>
U_macros("U", llvm::cl::value_desc("macro"), llvm::cl::Prefix,
         llvm::cl::desc("Undefine the specified macro"));

static llvm::cl::list<std::string>
ImplicitIncludes("include", llvm::cl::value_desc("file"),
                 llvm::cl::desc("Include file before parsing"));
static llvm::cl::list<std::string>
ImplicitMacroIncludes("imacros", llvm::cl::value_desc("file"),
                      llvm::cl::desc("Include macros from file before parsing"));

static llvm::cl::opt<std::string>
ImplicitIncludePTH("include-pth", llvm::cl::value_desc("file"),
                   llvm::cl::desc("Include file before parsing"));

//===----------------------------------------------------------------------===//
// PTH.
//===----------------------------------------------------------------------===//

static llvm::cl::opt<std::string>
TokenCache("token-cache", llvm::cl::value_desc("path"),
           llvm::cl::desc("Use specified token cache file"));

//===----------------------------------------------------------------------===//
// Preprocessor include path information.
//===----------------------------------------------------------------------===//

// This tool exports a large number of command line options to control how the
// preprocessor searches for header files.  At root, however, the Preprocessor
// object takes a very simple interface: a list of directories to search for
// 
// FIXME: -nostdinc++
// FIXME: -imultilib
//

static llvm::cl::opt<bool>
nostdinc("nostdinc", llvm::cl::desc("Disable standard #include directories"));

// Various command line options.  These four add directories to each chain.
static llvm::cl::list<std::string>
F_dirs("F", llvm::cl::value_desc("directory"), llvm::cl::Prefix,
       llvm::cl::desc("Add directory to framework include search path"));
static llvm::cl::list<std::string>
I_dirs("I", llvm::cl::value_desc("directory"), llvm::cl::Prefix,
       llvm::cl::desc("Add directory to include search path"));
static llvm::cl::list<std::string>
idirafter_dirs("idirafter", llvm::cl::value_desc("directory"), llvm::cl::Prefix,
               llvm::cl::desc("Add directory to AFTER include search path"));
static llvm::cl::list<std::string>
iquote_dirs("iquote", llvm::cl::value_desc("directory"), llvm::cl::Prefix,
               llvm::cl::desc("Add directory to QUOTE include search path"));
static llvm::cl::list<std::string>
isystem_dirs("isystem", llvm::cl::value_desc("directory"), llvm::cl::Prefix,
            llvm::cl::desc("Add directory to SYSTEM include search path"));

// These handle -iprefix/-iwithprefix/-iwithprefixbefore.
static llvm::cl::list<std::string>
iprefix_vals("iprefix", llvm::cl::value_desc("prefix"), llvm::cl::Prefix,
             llvm::cl::desc("Set the -iwithprefix/-iwithprefixbefore prefix"));
static llvm::cl::list<std::string>
iwithprefix_vals("iwithprefix", llvm::cl::value_desc("dir"), llvm::cl::Prefix,
     llvm::cl::desc("Set directory to SYSTEM include search path with prefix"));
static llvm::cl::list<std::string>
iwithprefixbefore_vals("iwithprefixbefore", llvm::cl::value_desc("dir"),
                       llvm::cl::Prefix,
            llvm::cl::desc("Set directory to include search path with prefix"));

static llvm::cl::opt<std::string>
isysroot("isysroot", llvm::cl::value_desc("dir"), llvm::cl::init("/"),
         llvm::cl::desc("Set the system root directory (usually /)"));

static cl::opt<bool> Verbose("verbose", cl::Optional, cl::init(false),
    cl::desc("Print out each action taken"));

static cl::alias VerboseAlias("v", cl::Optional,
    cl::desc("Alias for -verbose"), cl::aliasopt(Verbose));

static llvm::cl::opt<std::string>
Arch("arch", llvm::cl::desc("Specify target architecture (e.g. nios2)"));

// Finally, implement the code that groks the options above.

/// InitializeIncludePaths - Process the -I options and set them in the
/// HeaderSearch object.
void InitializeIncludePaths(const char *Argv0, HeaderSearch &Headers,
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
  llvm::sys::Path MainExecutablePath = 
     llvm::sys::Path::GetMainExecutable(Argv0, (void*)(intptr_t)InitializeIncludePaths);
  if (!MainExecutablePath.isEmpty()) {
    MainExecutablePath.eraseComponent();  // Remove prog   from foo/bin/prog
    MainExecutablePath.eraseComponent();  // Remove /bin   from foo/bin

    // Get foo/lib/ellcc/<version>/include    
    MainExecutablePath.appendComponent("libecc");
    MainExecutablePath.appendComponent(ELLCC_VERSION_STRING);
    MainExecutablePath.appendComponent(Arch);
    MainExecutablePath.appendComponent("include");
    
    // We pass true to ignore sysroot so that we *always* look for clang headers
    // relative to our executable, never relative to -isysroot.
    Init.AddPath(MainExecutablePath.c_str(), InitHeaderSearch::System,
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
    llvm::SmallVector<std::pair<unsigned, std::string*>, 8> OrderedPaths;

    if (!ImplicitIncludePTH.empty())
      OrderedPaths.push_back(std::make_pair(ImplicitIncludePTH.getPosition(),
                                            &ImplicitIncludePTH));
    for (unsigned i = 0, e = ImplicitIncludes.size(); i != e; ++i)
      OrderedPaths.push_back(std::make_pair(ImplicitIncludes.getPosition(i),
                                            &ImplicitIncludes[i]));
    llvm::array_pod_sort(OrderedPaths.begin(), OrderedPaths.end());


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
    llvm::OwningPtr<PTHManager> PTHMgr;

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
    llvm::OwningPtr<Preprocessor> PP(new Preprocessor(Diags, LO, TI,
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
    llvm::MemoryBuffer *SB = 
      llvm::MemoryBuffer::getMemBuffer(EmptyStr, EmptyStr, "<empty input>");
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
    llvm::MemoryBuffer *SB = llvm::MemoryBuffer::getSTDIN();

    // If stdin was empty, SB is null.  Cons up an empty memory
    // buffer now.
    if (!SB) {
      const char *EmptyStr = "";
      SB = llvm::MemoryBuffer::getMemBuffer(EmptyStr, EmptyStr, "<stdin>");
    }

    SourceMgr.createMainFileIDForMemBuffer(SB);
    if (SourceMgr.getMainFileID().isInvalid()) {
      PP.getDiagnostics().Report(FullSourceLoc(), 
                                 diag::err_fe_error_reading_stdin);
      return true;
    }
  }

  return false;
}


void doit(int argc, char **argv)
{
  // I think this is more noise than signal at this point
  xBase::logExceptions = false;

  traceAddSys("progress");
  //traceAddSys("parse-tree");

  if_malloc_stats();

  SourceLocManager mgr;

  // string table for storing parse tree identifiers
  StringTable strTable;

  // parsing language options
  LangOptions LO;
  LO.GNU_Cplusplus0x();
  OwningPtr<SourceManager> SourceMgr;
// Create a file manager object to provide access to and cache the filesystem.
  FileManager FileMgr;
  Diagnostic Diags;
  OwningPtr<TargetInfo> TI;
  TargetInfo *TI = TargetInfo::CreateTargetInfo(llvm::sys::getHostTriple());

  // ------------- process command-line arguments ---------
  char const *inputFname = myProcessArgs
    (argc, argv,
     "\n"
     "  general behavior flags:\n"
     "    c_lang             use C language rules (default is C++)\n"
     "    nohashline         ignore #line when reporting locations\n"
     "\n"
     "  options to stop after a certain stage:\n"
     "    stopAfterParse     stop after parsing\n"
     "    stopAfterTCheck    stop after typechecking\n"
     "    stopAfterElab      stop after semantic elaboration\n"
     "\n"
     "  output options:\n"
     "    parseTree          make a parse tree and print that, only\n"
     "    printAST           print AST after parsing\n"
     "    printTypedAST      print AST with type info\n"
     "    printElabAST       print AST after semantic elaboration\n"
     "    prettyPrint        echo input as pretty-printed C++\n"
     "    xmlPrintAST        print AST as XML\n"
     "\n"
     "  debugging output:\n"
     "    malloc_stats       print malloc stats every so often\n"
     "    env                print as variables are added to the environment\n"
     "    error              print as errors are accumulated\n"
     "    overload           print details of overload resolution\n"
     "\n"
     "  (grep in source for \"trace\" to find more obscure flags)\n"
     "");

  if (tracingSys("printAsML")) {
    Type::printAsML = true;
  }

  // FIX: dsw: couldn't we put dashes or something in here to break up
  // the word?
  if (tracingSys("nohashline")) {
    sourceLocManager->useHashLines = false;
  }

  if (tracingSys("tolerateHashlineErrors")) {
    sourceLocManager->tolerateHashlineErrors = true;
  }

  if (tracingSys("no-orig-offset")) {
    sourceLocManager->useOriginalOffset = false;
  }

  if (tracingSys("ansi")) {
    LO.ANSI_Cplusplus0x();
  }

  if (tracingSys("ansi_c")) {
    LO.ANSI_C89();
  }

  if (tracingSys("ansi_c99")) {
    LO.ANSI_C99();
  }

  if (tracingSys("c_lang")) {
    LO.GNU_C99();
  }

  if (tracingSys("gnu_c89")) {
    LO.ANSI_C89();
    LO.GNU_C_extensions();
  }

  if (tracingSys("gnu_kandr_c_lang")) {
    LO.GNU3_KandR_C();
    #ifndef KANDR_EXTENSION
      xfatal("gnu3_kandr_c_lang option requires the K&R module (./configure -kandr=yes)");
    #endif
  }

  if (tracingSys("gnu2_kandr_c_lang")) {
    LO.GNU2_KandR_C();
    #ifndef KANDR_EXTENSION
      xfatal("gnu2_kandr_c_lang option requires the K&R module (./configure -kandr=yes)");
    #endif
  }

  if (tracingSys("test_xfatal")) {
    xfatal("this is a test error message");
  }

  if (tracingSys("msvcBugs")) {
    LO.MSVC_bug_compatibility();
  }

  if (!tracingSys("nowarnings")) {
    LO.enableAllWarnings();
  }

  if (tracingSys("templateDebug")) {
    // predefined set of tracing flags I've been using while debugging
    // the new templates implementation
    traceAddSys("template");
    traceAddSys("error");
    traceAddSys("scope");
    traceAddSys("templateParams");
    traceAddSys("templateXfer");
    traceAddSys("prettyPrint");
    traceAddSys("xmlPrintAST");
    traceAddSys("topform");
  }

  if (tracingSys("only_works_on_32bit") &&
      sizeof(long) != 4) {
    // we are running a regression test, and the testcase is known to
    // fail due to dependence on architecture parameters, so skip it
    std::cout << "skipping test b/c this is not a 32-bit architecture\n";
    exit(0);
  }

  if (tracingSys("printTracers")) {
    std::cout << "tracing flags:\n\t";
    printTracers(std::cout, "\n\t");
    std::cout << std::endl;
  }

  // --------------- parse --------------
  TranslationUnit *unit;

  // Process the -I options and set them in the HeaderInfo.
  HeaderSearch HeaderInfo(FileMgr);
  InitializeIncludePaths(argv0, HeaderInfo, FileMgr, input.LO);
  // Set up the preprocessor with these options.
  DriverPreprocessorFactory PPFactory(Diags, input.LO, *TI,
                                      *SourceMgr.get(), HeaderInfo);
  OwningPtr<Preprocessor> PP(PPFactory.CreatePreprocessor());

  if (!PP)
      PrintAndExit("Can't create a preprocessor");
  
  if (InitializeSourceManager(*PP.get(), input.name.toString()))
      PrintAndExit("Can't initialize the source manager");

  // dsw: I needed this to persist past typechecking, so I moved it
  // out here.  Feel free to refactor.
  ArrayStack<Variable*> madeUpVariables;
  ArrayStack<Variable*> builtinVars;

  int parseWarnings = 0;
  long parseTime = 0;
  if (tracingSys("parseXml")) {
    if (tracingSys("parseXml-no-danglingPointers")) {
      xmlDanglingPointersAllowed = false;
    }
    unit = xmlDoRead(strTable, inputFname);
    if (!unit) return;
  }
  else {
    SectionTimer timer(parseTime);
    SemanticValue treeTop;
    ParseTreeAndTokens tree(PP, treeTop, strTable, inputFname);

    // grab the lexer so we can check it for errors (damn this
    // 'tree' thing is stupid..)
    Lexer *lexer = dynamic_cast<Lexer*>(tree.lexer);
    xassert(lexer);

    CCParse *parseContext = new CCParse(strTable, LO);
    tree.userAct = parseContext;

    traceProgress(2) << "building parse tables from internal data\n";
    ParseTables *tables = parseContext->makeTables();
    tree.tables = tables;

    maybeUseTrivialActions(tree);

    if (tracingSys("parseTree")) {
      // make some helpful aliases
      LexerInterface *underLexer = tree.lexer;
      UserActions *underAct = parseContext;

      // replace the lexer and parser with parse-tree-building versions
      tree.lexer = new ParseTreeLexer(underLexer, underAct);
      tree.userAct = new ParseTreeActions(underAct, tables);

      // 'underLexer' and 'tree.userAct' will be leaked.. oh well
    }

    if (!toplevelParse(tree, inputFname)) {
      exit(2); // parse error
    }

    // check for parse errors detected by the context class
    if (parseContext->errors || lexer->errors) {
      exit(2);
    }
    parseWarnings = lexer->warnings + parseContext->warnings;

    if (tracingSys("parseTree")) {
      // the 'treeTop' is actually a PTreeNode pointer; print the
      // tree and bail
      PTreeNode *ptn = (PTreeNode*)treeTop;
      ptn->printTree(std::cout, PTreeNode::PF_EXPAND);
      return;
    }

    // treeTop is a TranslationUnit pointer
    unit = (TranslationUnit*)treeTop;

    //unit->debugPrint(std::cout, 0);

    delete parseContext;
    delete tables;
  }

  checkHeap();

  // print abstract syntax tree
  if (tracingSys("printAST")) {
    unit->debugPrint(std::cout, 0);
  }

  //if (unit) {     // when "-tr trivialActions" it's NULL...
  //  std::cout << "ambiguous nodes: " << numAmbiguousNodes(unit) << std::endl;
  //}

  if (tracingSys("stopAfterParse")) {
    return;
  }


  // ---------------- typecheck -----------------
  BasicTypeFactory tfac;
  long tcheckTime = 0;
  if (tracingSys("no-typecheck")) {
    std::cout << "no-typecheck" << std::endl;
  } else {
    SectionTimer timer(tcheckTime);
    Env env(strTable, LO, *TI, tfac, madeUpVariables, builtinVars, unit);
    try {
      env.tcheckTranslationUnit(unit);
    }
    catch (XUnimp &x) {
      HANDLER();

      // relay to handler in main()
      std::cout << "in code near " << env.locStr() << ":\n";
      throw;
    }
    catch (x_assert &x) {
      HANDLER();

      if (env.errors.hasFromNonDisambErrors()) {
        if (tracingSys("expect_confused_bail")) {
          std::cout << "got the expected confused/bail\n";
          exit(0);
        }

        // The assertion failed only after we encountered and diagnosed
        // at least one real error in the input file.  Therefore, the
        // assertion might simply have been caused by a failure of the
        // error recovery code to fully restore all invariants (which is
        // often difficult).  So, we'll admit to being "confused", but
        // not to the presence of a bug in Elsa (which is what a failed
        // assertion otherwise nominally means).
        //
        // The error message is borrowed from gcc; I wouldn't be
        // surprised to discover they use a similar technique to decide
        // when to emit the same message.
        //
        // The reason I do not put the assertion failure message into
        // this one is I don't want it showing up in the output where
        // Delta might see it.  If I am intending to minimize an assertion
        // failure, it's no good if Delta introduces an error.
        env.error("confused by earlier errors, bailing out");
        env.errors.print(std::cout);
        exit(4);
      }

      if (tracingSys("expect_xfailure")) {
        std::cout << "got the expected xfailure\n";
        exit(0);
      }

      // if we don't have a basis for reducing severity, pass this on
      // to the normal handler
      handle_xBase(env, x);
    }
    catch (xBase &x) {
      HANDLER();
      handle_xBase(env, x);
    }

    int numErrors = env.errors.numErrors();
    int numWarnings = env.errors.numWarnings() + parseWarnings;

    // do this now so that 'printTypedAST' will include CFG info
    #ifdef CFG_EXTENSION
    // A possible TODO is to do this right after each function is type
    // checked.  However, in the current design, that means physically
    // inserting code into Function::tcheck (ifdef'd of course).  A way
    // to do it better would be to have a general post-function-tcheck
    // interface that analyses could hook in to.  That could be augmented
    // by a parsing mode that parsed each function, analyzed it, and then
    // immediately discarded its AST.
    if (numErrors == 0) {
      numErrors += computeUnitCFG(unit);
    }
    #endif // CFG_EXTENSION

    // print abstract syntax tree annotated with types
    if (tracingSys("printTypedAST")) {
      unit->debugPrint(std::cout, 0);
    }

    // structural delta thing
    if (tracingSys("structure")) {
      structurePrint(unit);
    }

    if (numErrors==0 && tracingSys("secondTcheck")) {
      // this is useful to measure the cost of disambiguation, since
      // now the tree is entirely free of ambiguities
      traceProgress() << "beginning second tcheck...\n";
      Env env2(strTable, LO, *TI, tfac, madeUpVariables, builtinVars, unit);
      unit->tcheck(env2);
      traceProgress() << "end of second tcheck\n";
    }

    // print errors and warnings
    env.errors.print(std::cout);

    std::cout << "typechecking results:\n"
         << "  errors:   " << numErrors << "\n"
         << "  warnings: " << numWarnings << "\n";

    if (numErrors != 0) {
      exit(4);
    }

    // lookup diagnostic
    if (env.collectLookupResults.length()) {
      // scan AST
      NameChecker nc;
      nc.sb << "collectLookupResults";
      unit->traverse(nc);

      // compare to given text
      if (streq(env.collectLookupResults, nc.sb)) {
        // ok
      }
      else {
        std::cout << "collectLookupResults do not match:\n"
             << "  source: " << env.collectLookupResults << "\n"
             << "  tcheck: " << nc.sb << "\n"
             ;
        exit(4);
      }
    }
  }

  // do this before elaboration; I just want to see the result of type
  // checking and disambiguation
  if (wantBpprint) {
    std::cout << "// bpprint\n";
    bppTranslationUnit(std::cout, *unit);
  }

  // ---------------- integrity checking ----------------
  long integrityTime = 0;
  {
    SectionTimer timer(integrityTime);

    // check AST integrity
    IntegrityVisitor ivis;
    unit->traverse(ivis);

    // check that the AST is a tree *and* that the lowered AST is a
    // tree; only do this *after* confirming that tcheck finished
    // without errors
    if (tracingSys("treeCheck")) {
      long start = getMilliseconds();
      LoweredIsTreeVisitor treeCheckVisitor;
      unit->traverse(treeCheckVisitor.loweredVisitor);
      traceProgress() << "done with tree check 1 ("
                      << (getMilliseconds() - start)
                      << " ms)\n";
    }

    // check an expected property of the annotated AST
    if (tracingSys("declTypeCheck") || getenv("declTypeCheck")) {
      DeclTypeChecker vis;
      unit->traverse(vis.loweredVisitor);
      std::cout << "instances of type != var->type: " << vis.instances << std::endl;
    }

    if (tracingSys("stopAfterTCheck")) {
      return;
    }
  }

  // ----------------- elaboration ------------------
  long elaborationTime = 0;
  if (tracingSys("no-elaborate")) {
    std::cout << "no-elaborate" << std::endl;
  }
  else {
    SectionTimer timer(elaborationTime);

    ElabVisitor vis(strTable, tfac, unit);

    if (!LO.CPlusPlus) {
      // do only the C elaboration activities
      vis.activities = EA_C_ACTIVITIES;
    }

    // if we are going to pretty print, then we need to retain defunct children
    if (tracingSys("prettyPrint")
        // dsw: I don't know if this is right, but printing the xml
        // AST kind of resembles pretty-printing the AST; fix this if
        // it is wrong
        || tracingSys("xmlPrintAST")
        || wantBpprintAfterElab
        ) {
      vis.cloneDefunctChildren = true;
    }

    // do elaboration
    unit->traverse(vis.loweredVisitor);

    // print abstract syntax tree annotated with types
    if (tracingSys("printElabAST")) {
      unit->debugPrint(std::cout, 0);
    }
    if (tracingSys("stopAfterElab")) {
      return;
    }
  }

  // mark "real" (non-template) variables as such
  if (!tracingSys("parseXml")) {
    // mark "real" (non-template) variables as such
    MarkRealVars markReal;
    visitVarsF(builtinVars, markReal);
    visitRealVarsF(unit, markReal);
  }

  // more integrity checking
  {
    SectionTimer timer(integrityTime);

    // check that the AST is a tree *and* that the lowered AST is a
    // tree (do this *after* elaboration!)
    if (tracingSys("treeCheck")) {
      long start = getMilliseconds();
      LoweredIsTreeVisitor treeCheckVisitor;
      unit->traverse(treeCheckVisitor.loweredVisitor);
      traceProgress() << "done with tree check 2 ("
                      << (getMilliseconds() - start)
                      << " ms)\n";
    }
  }

  // dsw: pretty printing
  if (tracingSys("prettyPrint")) {
    traceProgress() << "dsw pretty print...\n";
    std::cout << "---- START ----" << std::endl;
    std::cout << "// -*-c++-*-" << std::endl;
    prettyPrintTranslationUnit(std::cout, *unit);
    std::cout << "---- STOP ----" << std::endl;
    traceProgress() << "dsw pretty print... done\n";
  }

  if (wantBpprintAfterElab) {
    std::cout << "// bpprintAfterElab\n";
    bppTranslationUnit(std::cout, *unit);
  }

  // dsw: xml printing of the raw ast
  if (tracingSys("xmlPrintAST")) {
    traceProgress() << "dsw xml print...\n";
    bool indent = tracingSys("xmlPrintAST-indent");
    int depth = 0;              // shared depth counter between printers
    std::cout << "---- START ----" << std::endl;

    // serialize Files
    IdentityManager idmgr;
    XmlFileWriter fileXmlWriter(idmgr, &std::cout, depth, indent, NULL);
    fileXmlWriter.toXml(sourceLocManager->serializationOnly_get_files());

    // serialize AST and maybe Types
    if (tracingSys("xmlPrintAST-types")) {
      IdentityManager idmgr;
      XmlTypeWriter xmlTypeVis( idmgr, (ASTVisitor*)NULL, &std::cout, depth, indent, NULL );
      XmlTypeWriter_AstVisitor xmlVis_Types(xmlTypeVis, std::cout, depth, indent);
      xmlTypeVis.astVisitor = &xmlVis_Types;
      ASTVisitor *vis = &xmlVis_Types;
      LoweredASTVisitor loweredXmlVis(&xmlVis_Types); // might not be used
      if (tracingSys("xmlPrintAST-lowered")) {
        vis = &loweredXmlVis;
      }
      unit->traverse(*vis);
    } else {
      IdentityManager idmgr;
      XmlAstWriter_AstVisitor xmlVis(std::cout, idmgr, depth, indent);
      ASTVisitor *vis = &xmlVis;
      LoweredASTVisitor loweredXmlVis(&xmlVis); // might not be used
      if (tracingSys("xmlPrintAST-lowered")) {
        vis = &loweredXmlVis;
      }
      unit->traverse(*vis);
    }

    std::cout << std::endl;
    std::cout << "---- STOP ----" << std::endl;
    traceProgress() << "dsw xml print... done\n";
  }

  // test AST cloning
  if (tracingSys("testClone")) {
    TranslationUnit *u2 = unit->clone();

    if (tracingSys("cloneAST")) {
      std::cout << "------- cloned AST --------\n";
      u2->debugPrint(std::cout, 0);
    }

    if (tracingSys("cloneCheck")) {
      ArrayStack<Variable*> madeUpVariables2;
      ArrayStack<Variable*> builtinVars2;
      // dsw: I hope you intend that I should use the cloned TranslationUnit
      Env env3(strTable, LO, *TI, tfac, madeUpVariables2, builtinVars2, u2);
      u2->tcheck(env3);

      if (tracingSys("cloneTypedAST")) {
        std::cout << "------- cloned typed AST --------\n";
        u2->debugPrint(std::cout, 0);
      }

      if (tracingSys("clonePrint")) {
        OStreamOutStream out0(std::cout);
        CodeOutStream codeOut(out0);
        CTypePrinter typePrinter;
        PrintEnv penv(typePrinter, &codeOut);
        std::cout << "---- cloned pretty print ----" << std::endl;
        u2->print(penv);
        codeOut.finish();
      }
    }
  }

  // test debugPrint but send the output to /dev/null (basically just
  // make sure it doesn't segfault or abort)
  if (tracingSys("testDebugPrint")) {
    ofstream devnull("/dev/null");
    unit->debugPrint(devnull, 0);
  }

  std::cout << "parse=" << parseTime << "ms"
       << " tcheck=" << tcheckTime << "ms"
       << " integ=" << integrityTime << "ms"
       << " elab=" << elaborationTime << "ms"
       << "\n"
       ;

  if (!cc2cOutputFname.empty()) {
    TranslationUnit *lowered = cc_to_c(strTable, *unit);
    if (cc2cOutputFname == sm::string("-")) {
      std::cout << "// cc2c\n";
      bppTranslationUnit(std::cout, *lowered);
    }
    else {
      ofstream out(cc2cOutputFname.c_str());
      if (!out) {
        xsyserror("open", stringb("write \"" << cc2cOutputFname << "\""));
      }
      out << "// cc2c\n";
      bppTranslationUnit(out, *lowered);
    }
  }

#ifdef LLVM_EXTENSION
  if (!cc2llvmOutputFname.empty()) {
    TargetInfo* TI = TargetInfo::CreateTargetInfo("i686-pc-linux-gnu");
    llvm::Module* mod = cc_to_llvm(cc2llvmOutputFname, strTable, *unit, *TI);

    // Output the module.
    llvm::PassManager PM;
    std::ostream *out = &std::cout;  // Default to printing to stdout.
    if (cc2llvmOutputFname != sm::string("-")) {
      out = new std::ofstream(cc2llvmOutputFname.c_str());
      if (!out->good()) {
        xsyserror("open", stringb("write \"" << cc2llvmOutputFname << "\""));
      }
    }
    llvm::raw_os_ostream L(*out);
    PM.add(createPrintModulePass(&L));
    PM.run(*mod);
    if (out != &std::cout) {
      ((std::ofstream*)out)->close();
      delete out;
    }
    delete mod;
  }
#endif

  //traceProgress() << "cleaning up...\n";

  //malloc_stats();

  // delete the tree
  // (currently this doesn't do very much because FakeLists are
  // non-owning, so I won't pretend it does)
  //delete unit;

  strTable.clear();

  //checkHeap();
  //malloc_stats();
}

int main(int argc, char **argv)
{
  progname = argv[0];
  try {
    doit(argc, argv);
  }
  catch (XUnimp &x) {
    HANDLER();
    std::cout << x << std::endl;

    // don't consider this the same as dying on an assertion failure;
    // I want to have tests in regrtest that are "expected" to fail
    // for the reason that they use unimplemented language features
    return 10;
  }
  catch (XFatal &x) {
    HANDLER();

    // similar to XUnimp
    std::cout << x << std::endl;
    return 10;
  }
  catch (xBase &x) {
    HANDLER();
    std::cout << x << std::endl;
    abort();
  }

  //malloc_stats();

  return 0;
}
