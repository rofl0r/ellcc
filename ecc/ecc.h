//===- ecc.h - The Embedded Compiler Collection driver -----------------------------------===//
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

#ifndef _ECC_H_
#define _ECC_H_

#include "llvm/Support/Timer.h"

namespace ecc {

class ECC {
public:
    ECC(llvm::TimerGroup& timerGroup);
    ~ECC();

private:
    /** The default constructor (not implemented or used).
     */
    ECC();

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

#endif  // _ECC_H_
