// notopt.cc
// little routines that GCC's optimizer barfs on (produces incorrect
// code), so I separate them from the rest of the code (very annoying)

#include "cc_env.h"                     // Env
#include "trace.h"                      // TRACE
#include "Diagnostic.h"

using namespace ellcc;

// I am certain it is broken on gcc-2.95.3 on x86; maybe it
// is ok on gcc-3, maybe not ...
#if defined(__GNUC__) && __GNUC__<3 && defined(__OPTIMIZE__)
  #error This module must be compiled with optimization turned off
#endif

void checkAsmLabel(Env& env, SourceLocation loc, Variable* var, Declarator::Tcheck&dt)
{
    if (dt.asmname) {
        // Have an asm label, check some things.
        if (!var->isSemanticallyGlobal() && !(var->flags & DF_REGISTER)) {
            env.report(loc, diag::err_asm_label_on_a_non_static_local_variable)
                << dt.asmname;
            return;
        }
        if (var->asmname && var->asmname != dt.asmname) {
            env.report(loc, diag::err_asm_label_does_not_match_previous)
                << dt.asmname << var->name << var->asmname;
            return;
        }
        if ((var->flags & DF_REGISTER) && !env.TI.isValidGCCRegisterName(dt.asmname)) {
            env.report(loc, diag::err_asm_label_is_not_a_valid_register)
                << dt.asmname << var->name
                << env.TI.getTargetPrefix();
            return;
        }
 
        // Set the asm label.
        var->asmname = dt.asmname;
    }
}

// This is the main one that initially caused the problem.  With
// -O2, an XTypeDeduction just flies right by the 'catch' as if
// it weren't there.
Type *Env::applyArgumentMapToType_helper(MType &map, Type *origSrc)
{
  try {
    return applyArgumentMapToType(map, origSrc);
  }
  catch (XTypeDeduction &x) {
    HANDLER();
    TRACE("template", "failure to instantiate: " << x.why());
    return NULL;
  }
}


// This is the only other function that catches XTypeDeduction,
// so I moved it out here too.
STemplateArgument *Env::makeDefaultTemplateArgument
  (Variable const *param, MType &map)
{
  // type parameter?
  if (param->hasFlag(DF_TYPEDEF) &&
      param->defaultParamType) {
    // use 'param->defaultParamType', but push it through the map
    // so it can refer to previous arguments 
    try {
      Type *t = applyArgumentMapToType(map, param->defaultParamType);
      return new STemplateArgument(t);
    }
    catch (XTypeDeduction &x) {
      HANDLER();
      report(loc(), diag::err_template_could_not_evaluate_default_argument)
          << param->defaultParamType->toString() << x.why();
      return NULL;
    }
  }
  
  // non-type parameter?
  else if (!param->hasFlag(DF_TYPEDEF) &&
           param->value) {
    try {
      STemplateArgument *ret = new STemplateArgument;
      *ret = applyArgumentMapToExpression(map, param->value);
      return ret;
    }
    catch (XTypeDeduction &x) {
      HANDLER();
      report(loc(), diag::err_template_could_not_evaluate_default_argument)
          << param->value->exprToString() << x.why();
      return NULL;
    }
  }

  return NULL;
}


// EOF
