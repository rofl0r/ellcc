// agrampar.cc            see license.txt for copyright and terms of use
// code for agrampar.h

#include "agrampar.h"        // this module
#include "agrampar.tab.h"    // YYSTYPE union
#include "gramlex.h"         // GrammarLexer
#include "exc.h"             // xformat
#include "trace.h"           // tracing debugging functions
#include "owner.h"           // Owner
#include "strutil.h"         // trimWhitespace
#include "strtable.h"        // StringTable
#include "FileManager.h"
#include "SourceManager.h"
#include "llvm/Support/MemoryBuffer.h"

#include <string.h>          // strncmp
#include <ctype.h>           // isalnum
#include <sstream>           // istringstream

using namespace sm;

string unbox(string *s)
{
  string ret = *s;      // later optimization: transfer s.p directly
  delete s;
  return ret;
}

string *box(char const *s)
{
  return new string(s);
}

string *appendStr(string *left, string *right)
{
  string *ret = new string(*left & *right);
  delete left;
  delete right;
  return ret;
}


CtorArg *parseCtorArg(rostring origStr)
{
  CtorArg *ret = new CtorArg(false, "", "", "");

  // strip leading and trailing whitespace
  string str = trimWhitespace(origStr);

  // check for owner flag
  if (prefixEquals(str, "owner")) {
    ret->isOwner = true;
    str = str.substring(6, str.length() - 6);    // skip "owner "
  }

  // check for an initial value
  char const *equals = strchr(str.c_str(), '=');
  if (equals) {
    ret->defaultValue = equals+1;
    str = trimWhitespace(str.substring(0, equals-str.c_str()));
    trace("defaultValue") << "split into `" << str
                          << "' and `" << ret->defaultValue << "'\n";
  }

  // work from the right adge, collecting alphanumerics into the name;
  // this restricts the kinds of C type syntaxes we allow, but you can use
  // typedefs to express any type within these restrictions
  char const *start = str.c_str();
  char const *p = start + str.length() - 1;
  while ((isalnum(*p) || *p == '_') && p > start) {
    p--;
  }
  if (p == start) {
    xformat(stringc << "missing type specifier in \"" << origStr << "\"");
  }
  p++;

  ret->type = trimWhitespace(substring(start, p-start));
  ret->name = trimWhitespace(string(p));

  return ret;
}


// Bison parser calls this to get a token
int agrampar_yylex(YYSTYPE *lvalp, void *parseParam)
{
  ASTParseParams *par = (ASTParseParams*)parseParam;
  GrammarLexer &lexer = par->lexer;

  int code = lexer.yylexInc();

  // yield semantic values for some things
  switch (code) {
    case TOK_NAME:
    case TOK_INTLIT:
      lvalp->str = box(lexer.curToken());
      break;

    case TOK_EMBEDDED_CODE:
      lvalp->str = box(lexer.curFuncBody());
      break;

    default:
      lvalp->str = NULL;
  }

  static bool traceIt = tracingSys("tokens");
  if (traceIt) {
    std::ostream &os = trace("tokens");
    os << lexer.curLocStr() << ": " << code;
    if (lvalp->str) {
      os << ", \"" << *(lvalp->str) << "\"";
    }
    os << "\n";
  }

  return code;
}


void agrampar_yyerror(char const *msg, void *parseParam)
{
  ((ASTParseParams*)parseParam)->lexer.err(msg);
}


// ---------------- external interface -------------------
StringTable stringTable;

ASTSpecFile *readAbstractGrammar(char const *fname)
{
  if (tracingSys("yydebug")) {
    #ifndef NDEBUG
      yydebug = true;
    #else
      std::cout << "debugging disabled by -DNDEBUG\n";
    #endif
  }

  ellcc::SourceManager SM;
  ellcc::FileManager FM;
  if (fname == NULL) {
    // Read from stdin.
    llvm::MemoryBuffer *SB = llvm::MemoryBuffer::getSTDIN();

    // If stdin was empty, SB is null.  Cons up an empty memory
    // buffer now.
    if (!SB) {
      const char *EmptyStr = "";
      fname = "<stdin>";
      SB = llvm::MemoryBuffer::getMemBuffer(EmptyStr, EmptyStr, fname);
    }

    SM.createMainFileIDForMemBuffer(SB);
  } else {
    // Reading from a file.
    const ellcc::FileEntry *File = FM.getFile(fname);
    if (File) SM.createMainFileID(File, ellcc::SourceLocation());
    if (SM.getMainFileID().isInvalid()) {
      xformat(stringc << "could not read \"" << fname << "\"");
    }
  }

  const llvm::MemoryBuffer *MB = SM.getBuffer(SM.getMainFileID());
  Owner<std::istringstream> in;
  in = new std::istringstream();
  in->str(MB->getBufferStart());
  Owner<GrammarLexer> lexer;
  lexer = new GrammarLexer(isAGramlexEmbed, stringTable, fname, in.xfr());

  ASTParseParams params(*lexer);

  traceProgress() << "parsing grammar source..\n";
  int retval;
  try {
    retval = agrampar_yyparse(&params);
  }
  catch (xFormat &x) {
    lexer->err(x.cond());     // print with line number info
    throw;
  }

  if (retval == 0) {
    return params.treeTop;
  }
  else {
    xformat("parsing finished with an error");
  }
}

// ----------------------- test code -----------------------
#ifdef TEST_AGRAMPAR

#include "test.h"    // ARGS_MAIN

void entry(int argc, char **argv)
{
  TRACE_ARGS();

  if (argc != 2) {
    cout << "usage: " << argv[0] << " ast-spec-file\n";
    return;
  }

  // parse the grammar spec
  Owner<ASTSpecFile> ast;
  ast = readAbstractGrammar(argv[1]);

  // print it out
  ast->debugPrint(cout, 0);
}

ARGS_MAIN

#endif // TEST_AGRAMPAR
