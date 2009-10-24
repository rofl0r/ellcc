// mangle.h
// name mangling

// For now, this is just a verson of Type::toString that does
// not print parameter names.  It's a hack.

// Eventually, it should:
//   - mangle names compactly
//   - demangle names back into types
//   - support more than one algorithm (gcc's, for example) (?)


#ifndef MANGLE_H
#define MANGLE_H

#include "str.h"             // string, stringBuilder
#include "objlist.h"         // ObjList

class Type;                  // cc_type.h
class AtomicType;            // cc_type.h
class Variable;              // variable.h
class TemplateInfo;          // template.h
class STemplateArgument;     // template.h


// main entry point
sm::string mangle(Type const *t);


// helpers
sm::string mangleAtomic(AtomicType const *t);
sm::string leftMangle(Type const *t, bool innerParen = true);
sm::string rightMangle(Type const *t, bool innerParen = true);
sm::string mangleVariable(Variable const *v);
sm::string mangleTemplateParams(TemplateInfo const *tp);


#endif // MANGLE_H
