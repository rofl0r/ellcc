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
#include "cc_env.h"             // Env
#if RICH
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

/** Type check a state definition.
 * A state definition consists of a list of lists of StateEntries.
 * The first list has one list for each AND state in the state.
 * There may be, and often is, only one state list. if the state
 * is not an AND state.
 * The second list are the various entries to be performed in that state.
 */
static void checkState(Env& env, State* s)
{
    if (env.secondPassTcheck) {
        FAKELIST_FOREACH(StateEntryList, s->states, state) {
            FAKELIST_FOREACH_NC(StateEntry, state->entries, entry) {
                // Type check each state entry.
                entry->tcheck(env);
            }
        }
    }
}

void MR_state::tcheck(Env &env)
{
    // Type check the state definition.
    checkState(env, s);
}

// RICH: Dummies for now.
void MR_state::print(PrintEnv &env) { }
void MR_state::print(BPEnv &bp) const { }

/** A state defined at the outermost level.
 * The syntax allows a global state to be marked as an initial state,
 * but this is just for error checking. It makes no sense for a global
 * state to be marked as an initial state: It must be done where the
 * machine is defined, in a struct/class.
 */
void TF_state::itcheck(Env &env)
{
    if (s->initial) {
        env.report(s->loc, diag::err_state_global_initial_state)
            << s->d->getDeclaratorId()->getName()
            << SourceRange(s->loc, s->d->endloc);
        s->initial = false;
    }

    // Type check the state definition.
    checkState(env, s);
}

void TF_state::print(PrintEnv &env) { }
void TF_state::print(BPEnv &bp) const { }

void S_transition::itcheck(Env &env) { }
void S_transition::iprint(PrintEnv &env) { }
void S_transition::iprint(BPEnv &env) const { }
void S_transition::icfg(CFGEnv &bp) { }
void S_transition::cc2c(CC2CEnv &env) const { }
void S_transition::cc2llvm(CC2LLVMEnv &env) const { }

/** Type check a timeout definition.
 */
void SE_timer::tcheck(Env &env)
{
    e->tcheck(env, e);                          // Check the timeout expression.
    s->itcheck(env);                            // Check the timeout statement.
}

/** Type check an entry action.
 */
void SE_entry::tcheck(Env &env)
{
    s->itcheck(env);                            // Check the entry statement.
}

/** Type check a event handler.
 */
void SE_event::tcheck(Env &env)
{
    // Get declarator info.
    Declarator::Tcheck check(env.getSimpleType(ST_VOID), DF_NONE, DC_UNKNOWN);
    d->tcheck(env, check);              // Check the event declarator.
    s->itcheck(env);                    // Check the event handler statement.
}

/** Type check the default handler.
 */
void SE_default::tcheck(Env &env)
{
    s->itcheck(env);                            // Check the default statement.
}

/** Type check a nest state or state reference.
 */
void SE_reference::tcheck(Env &env)
{
    // Type check the state definition or reference.
    checkState(env, s);
}

/** Type check an exit action.
 */
void SE_exit::tcheck(Env &env)
{
    s->itcheck(env);                            // Check the exit statement.
}
