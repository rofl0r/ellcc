// ast_build.h
// code for ast_build.cc

#include "ast_build.h"      // this module
#include "exprloc.h"

Expression *makeRval(Expression *e)
{
  if (0 && e->type->isReference()) {	// RICH: causes t0020.cc to fail.
    E_stdConv *ret = new E_stdConv(EXPR_LOC(e->loc ENDLOCARG(SL_UNKNOWN)) e, SC_LVAL_TO_RVAL);
    ret->type = e->type->asRval();
    return ret;
  }
  else {
    return e;
  }
}


FakeList<ArgExpression> *makeExprList1(Expression *e)
{
  return FakeList<ArgExpression>::makeList(new ArgExpression(makeRval(e)));
}

FakeList<ArgExpression> *makeExprList2(Expression *e1, Expression *e2)
{
  return makeExprList1(e2)->prepend(new ArgExpression(makeRval(e1)));
}


// EOF
