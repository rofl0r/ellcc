#ifndef _Elsa_h_
#define _Elsa_h_

#include <llvm/Module.h>

class Elsa {
public:
    Elsa();
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
    void setup();

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
    int doit(Language language, const char* inputFname, const char* outputFname, llvm::Module*& mod);
};

#endif
