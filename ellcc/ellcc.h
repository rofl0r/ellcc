//===- ellcc.h - The Elsa<->LLVM driver -----------------------------------===//
//
// Copyright (c) 2009, Richard Pennington
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//===----------------------------------------------------------------------===//

#ifndef _Elsa_h_
#define _Elsa_h_

namespace llvm {
    class LLVMContext;
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

    /** Parse a source file.
     */
    int parse(Preprocessor& PP,
              const char* inputFname, const char* outputFname, llvm::Module*& mod,
              bool parseOnly, llvm::LLVMContext& context, LangOptions& LO, bool debug);

private:
    /** The default constructor (not implemented or used).
     */
    Elsa();
    int doit(Preprocessor& PP,
             const char* inputFname, const char* outputFname, llvm::Module*& mod,
             bool parseOnly, llvm::LLVMContext& context, LangOptions& LO, bool debug);
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
