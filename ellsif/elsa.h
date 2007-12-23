#ifndef _Elsa_h_
#define _Elsa_h_

namespace llvm {
    class Module;
};

#include "llvm/Support/Timer.h"

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
    int parse(Language language, const char* inputFname, const char* outputFname, llvm::Module*& mod);

private:
    /** The default constructor (not implemented or used).
     */
    Elsa();
    int doit(Language language, const char* inputFname, const char* outputFname, llvm::Module*& mod);
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

#endif
