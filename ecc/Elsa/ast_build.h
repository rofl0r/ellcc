// ast_build.h
// some utilities for constructing fragments of the C++ AST

#ifndef AST_BUILD_H
#define AST_BUILD_H

#include "cc_ast.h"      // C++ AST

// turn 'e' into an rvalue; no-op if it already is
Expression *makeRval(Expression *e);

// wrap an expression in a list of rvalues
FakeList<ArgExpression> *makeExprList1(Expression *e);

// wrap a pair of expressions in a list of rvalues
FakeList<ArgExpression> *makeExprList2(Expression *e1, Expression *e2);

#endif // AST_BUILD_H
