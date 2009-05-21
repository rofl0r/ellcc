#ifndef _Elsa_h_
#define _Elsa_h_

namespace llvm {
    class Module;
};

namespace pw {
    class Plexer;
};

#include "TargetInfo.h"
#include "LangOptions.h"

#include "llvm/Support/Timer.h"

namespace ellcc {
class Preprocessor;

/// DoPrintPreprocessedInput - Implement -E mode.
void DoPrintPreprocessedInput(Preprocessor &PP, const std::string& OutFile);
bool ProcessWarningOptions(Diagnostic &Diags);

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
        GNUKANDRC,      // GNU K&R C
    };
    /** Parse a source file.
     */
    int parse(Language language, const char* inputFname, const char* outputFname, llvm::Module*& mod, pw::Plexer* lang,
              TargetInfo* targetInfo);

private:
    /** The default constructor (not implemented or used).
     */
    Elsa();
    int doit(Language language, const char* inputFname, const char* outputFname, llvm::Module*& mod,
             TargetInfo* targetInfo);
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
