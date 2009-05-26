#ifndef _Elsa_h_
#define _Elsa_h_

namespace llvm {
    class Module;
};

namespace pw {
    class Plexer;
};

#include "llvm/Support/Timer.h"

namespace ellcc {
class Preprocessor;
class LangOptions;
class TargetInfo;
class Diagnostic;

/// DoPrintPreprocessedInput - Implement -E mode.
void DoPrintPreprocessedInput(Preprocessor &PP, const std::string& OutFile);
bool ProcessWarningOptions(Diagnostic &Diags);
bool CreateDependencyFileGen(Preprocessor *PP, std::string &ErrStr);
void CacheTokens(Preprocessor &PP, const std::string &OutFile);
bool CheckDiagnostics(Preprocessor &PP);

class Elsa {
public:
    Elsa(llvm::TimerGroup& timerGroup);
    ~Elsa();

    /** Print the tchecked C++ syntax using bpprint after tcheck.
     */
    bool wantBpprint;
    /** Same, but after elaboration.
     */
    bool wantBpprintAfterElab;
    /** Add trace systems.
     */
    void addTrace(const char* systems);
    /** Set up after command line parsing.
     */
    void setup(bool time);

    enum Language {
        GNUCXX,         // GNU C++
        ANSICXX,        // ANSI C++
        ANSIC89,        // ANSI C89
        ANSIC99,        // ANSI C99
        GNUC,           // GNU C
        GNUC89,         // GNU C89
        KANDRC,         // K&R C
        GNU2KANDRC,     // GNU2 K&R C
        GNU3KANDRC,     // GNU3 K&R C
    };
    /** Parse a source file.
     */
    int parse(Preprocessor& PP, LangOptions& LO, TargetInfo& TI,
              Language language,
              const char* inputFname, const char* outputFname, llvm::Module*& mod);

private:
    /** The default constructor (not implemented or used).
     */
    Elsa();
    int doit(Preprocessor& PP, LangOptions& LO, TargetInfo& TI,
             Language language,
             const char* inputFname, const char* outputFname, llvm::Module*& mod);
    /** true if phases should be timed.
     */
    bool doTime;
public:
    llvm::TimerGroup& timerGroup;
    llvm::Timer parseTimer;
    llvm::Timer typeCheckingTimer;
    llvm::Timer elaborationTimer;
    llvm::Timer integrityCheckingTimer;
    llvm::Timer llvmGenerationTimer;
};

}  // end of namespace ellcc
#endif
