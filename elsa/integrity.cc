// integrity.cc
// code for integrity.h

#include "integrity.h"         // this module


void IntegrityVisitor::foundAmbiguous(void *obj, void **ambig, char const *kind)
{
  // 2005-06-29: I have so far been unable to provoke this error by
  // doing simple error seeding, because it appears to be masked by a
  // check in the elaboration visitor regarding visiting certain lists
  // more than once.  Among other things, that means if there is a bug
  // along these lines, a user will discover it by seeing the
  // unfriendly list-visit assertion failure instead of the message
  // here.  But, that stuff is wrapped up in the Daniel's lowered
  // visitor mechanism, which I don't want to mess with now.  Anyway,
  // I'm reasonably confident that this check will work properly.
  xfatal(toString(loc) << ": internal error: found ambiguous " << kind);
}


bool IntegrityVisitor::visitDeclarator(Declarator *obj)
{
  if (!ASTVisitorEx::visitDeclarator(obj)) {
    return false;
  }

  // make sure the type is not a DQT if we are not in a template
  if (!inTemplate) {
    checkNontemplateType(obj->var->type);
    checkNontemplateType(obj->type);
  }

  return true;
}

void IntegrityVisitor::checkNontemplateType(Type *t)
{
#if RICH
  // RICH: This causes t0224.cc to fail.
  if (t->containsGeneralizedDependent()) {
    xfatal(toString(loc) << ": internal error: found dependent type `"
                         << t->toString() << "' in non-template (0a257264-c6ec-4983-95d0-fcd6aa48a6ce)");
  }
#endif
}


bool IntegrityVisitor::visitExpression(Expression *obj)
{
  if (!ASTVisitorEx::visitExpression(obj)) {
    return false;
  }

#if RICH
  // RICH: This causes t0093.cc to fail.
  if (!inTemplate) {
    xassert(obj->type != NULL);

    if (obj->isE_grouping()) {
      xfatal(toString(loc) << ": internal error: found E_grouping after tcheck");
    }
    if (obj->isE_arrow()) {
      xfatal(toString(loc) << ": internal error: found E_arrow after tcheck");
    }
  }
#else
  // why was I not doing this before?  detects problem in/t0584.cc
  if (!inTemplate && obj->type) {
    checkNontemplateType(obj->type);
  }
#endif

  return true;
}


// EOF
