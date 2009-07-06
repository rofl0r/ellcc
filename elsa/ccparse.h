// ccparse.h            see license.txt for copyright and terms of use
// data structures used during parsing of C++ code

#ifndef CCPARSE_H
#define CCPARSE_H

#include "strhash.h"            // StringHash
#include "strtable.h"           // StringTable
#include "objlist.h"            // ObjList
#include "array.h"              // ArrayStack
#include "cc_flags.h"           // UberModifiers, SimpleTypeId
#include "cc_ast.h"             // C++ AST classes, needed for the action function signatures
#include "ElsaDiagnostic.h"
#include "Preprocessor.h"       // Preprocessor
#include "SourceLocation.h"     // SourceLocation
#include "LangOptions.h"        // bool3

using namespace ellcc;

// parsing action state
class ParseEnv {
private:
  ArrayStack<StringRef> classNameStack;   // stack of class names

public:
  StringTable &str;                       // string table
  StringRef strRefAttr;                   // "attr"
  ellcc::Preprocessor& PP;                // The preprocessor.

public:
  ParseEnv(StringTable &table, ellcc::Preprocessor& PP)
    : classNameStack(),
      str(table),
      strRefAttr(table.add("attr")),
      PP(PP),
      diag(PP.getDiagnostics())
   {}
  ~ParseEnv() {}

  // manipulate the name of the class whose declaration we're inside;
  // this is *not* the general class context, merely the innermost
  // syntactically-occurring "class { ... }" declaration syntax, and
  // it is used only to recognize declarations of constructors
  void pushClassName(StringRef n);
  void popClassName();
  StringRef curClassName() const   { return classNameStack.top(); }

  // manipulate UberModifiers
  SimpleTypeId uberSimpleType(SourceLocation loc, UberModifiers m);
  UberModifiers uberCombine(SourceLocation loc, UberModifiers m1, UberModifiers m2);

  // generate a LocString suitable for use during parsing
  LocString * /*owner*/ ls(SourceLocation loc, char const *name);

  // Diagnotics.
  Diagnostic& diag;
  DiagnosticBuilder report(SourceLocation loc, unsigned DiagID);
  void diagnose3(bool3 b, SourceLocation L, unsigned DiagID, unsigned NoteID = 0);
};


// count ambiguous nodes; actually, this may double-count some nodes
// because of sharing in the ambiguous forest--but it will
// double-count exactly to the degree that debugPrint will
// double-print, so they will agree in that regard
int numAmbiguousNodes(TranslationUnit *unit);
int numAmbiguousNodes(Statement *stmt);
int numAmbiguousNodes(Expression *e);
int numAmbiguousNodes(ASTTypeId *t);


#if 0   // not needed anymore
// fail if there are any ambiguous nodes
void rejectAmbiguousNodes(TranslationUnit *unit);


// search within an AST for a location
class LocationSearcher : public ASTVisitor {
public:
  // location, or SL_UNKNOWN if not found yet
  SourceLocation loc;

public:
  LocationSearcher();

  // all nodes with locations
  #define DECL(type) \
    bool visit##type(type *obj) /*user ;*/
  DECL(TopForm);
  DECL(PQName);
  DECL(TypeSpecifier);
  DECL(Enumerator);
  DECL(Member);
  DECL(IDeclarator);
  DECL(Statement);
  DECL(Initializer);
  DECL(TemplateParameter);
  #undef DECL
};

// use the searcher to retrieve the location for an arbitrary
// AST node (may return SL_UNKNOWN if it cannot find one)
template <class T>
SourceLocation getASTNodeLoc(T *obj)
{
  LocationSearcher searcher;
  obj->traverse(searcher);
  return searcher.loc;
}
#endif // 0


#endif // CCPARSE_H
