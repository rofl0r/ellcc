// state.cc
// tcheck and print routines for state.ast/state.gr extensions

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

