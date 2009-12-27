//===- state.cc - State machine extensions ------------------------------===//
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

#include "generic_aux.h"        // C++ AST, and genericPrintAmbiguities, etc.
#if RICH
#include "cc_env.h"             // Env
#include "cc_print.h"           // olayer, PrintEnv
#include "generic_amb.h"        // resolveAmbiguity, etc.
#include "stdconv.h"            // usualArithmeticConversions
#include "astvisit.h"           // ASTVisitorEx
#include <string.h>             // strcmp
#include "datablok.h"
#include "TargetInfo.h"
#endif

using namespace ellcc;
using namespace std;

// RICH: Dummies for now.
void MR_state::tcheck(Env &env) { }
void MR_state::print(PrintEnv &env) { }
void MR_state::print(BPEnv &bp) const { }

void TF_state::itcheck(Env &env) { }
void TF_state::print(PrintEnv &env) { }
void TF_state::print(BPEnv &bp) const { }

void S_transition::itcheck(Env &env) { }
void S_transition::iprint(PrintEnv &env) { }
void S_transition::iprint(BPEnv &env) const { }
void S_transition::icfg(CFGEnv &bp) { }
void S_transition::cc2c(CC2CEnv &env) const { }
void S_transition::cc2llvm(CC2LLVMEnv &env) const { }

void SE_exit::tcheck(Env &env) { }
void SE_reference::tcheck(Env &env) { }
void SE_default::tcheck(Env &env) { }
void SE_event::tcheck(Env &env) { }
void SE_entry::tcheck(Env &env) { }
void SE_timer::tcheck(Env &env) { }
