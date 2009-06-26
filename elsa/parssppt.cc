// parssppt.cc            see license.txt for copyright and terms of use
// code for parssppt.h

#include "parssppt.h"     // this module
#include "glr.h"          // toplevelParse
#include "trace.h"        // traceProcessArg
#include "syserr.h"       // xsyserror
#include "Preprocessor.h"
#include "LiteralSupport.h"
#include "Token.h"
#include "cc_tokens.h"
#include "lexerint.h"     // LexerInterface
#include <stdlib.h>       // exit

using namespace std;
using namespace ellcc;

class PPLexer : public LexerInterface {
public:
    PPLexer(Preprocessor& PP, StringTable& ST)
        : PP(PP), ST(ST)
    {
        // Prime this lexer with the first token.
        getTokenFunc()(this);
    }
    NextTokenFunc getTokenFunc() const;
    sm::string tokenDesc() const
        { return tokenKindDesc(token.getKind()); }
    sm::string tokenKindDesc(int kind) const
        { 
            const char* name = tok::getTokenSimpleSpelling((tok::TokenKind)kind);
            if (name == NULL) {
                name = tok::getTokenName((tok::TokenKind)kind);
               return name;
            } else {
                return stringc << "'" <<  name << "'";
            }
        }
private:
    Preprocessor& PP;
    Token token;
    static void Lex(LexerInterface *li);
    StringTable& ST;
};

PPLexer::NextTokenFunc PPLexer::getTokenFunc() const
{
    return Lex;
}

/** The interface to the preprocessor. Fill in the LexerInterface fields.
 */
void PPLexer::Lex(LexerInterface *li)
{
    PPLexer& LI = *static_cast<PPLexer*>(li);
    LI.PP.Lex(LI.token);                        // Get the next token.
    if (LI.token.getKind() == TOK(kw___extension__)) {
        // Ignore extension.
        LI.PP.Lex(LI.token);                    // Get the next token.
    }
    LI.type = LI.token.getKind();
    if (LI.token.getKind() == TOK(numeric_constant)) {
        llvm::SmallString<64> IntegerBuffer;
        IntegerBuffer.resize(LI.token.getLength());
        const char *ThisTokBegin = &IntegerBuffer[0];
        unsigned ActualLength = LI.PP.getSpelling(LI.token, ThisTokBegin);
        NumericLiteralParser Literal(ThisTokBegin, ThisTokBegin+ActualLength, 
                                     LI.token.getLocation(), LI.PP);
        if (!Literal.isFloatingLiteral()) {
            LI.type = TOK_INT_LITERAL;
        }
    }
    if (   LI.token.getKind() == TOK(wide_string_literal)
        || LI.token.getKind() == TOK(wide_string_literal)) {
            // RICH: For now, just call these strings.
            LI.type = TOK_STRING_LITERAL;
    }
    if (LI.token.isLiteral()) {
        LI.sval = (SemanticValue)LI.ST.add(LI.token.getLiteralData(), LI.token.getLength());
    } else if (LI.token.isAnnotation()) {
        LI.sval = (SemanticValue)LI.token.getAnnotationValue();
    } else {
        IdentifierInfo* II = LI.token.getIdentifierInfo();
        if (II) {
            LI.sval = (SemanticValue)LI.ST.add(II->getName());
        } else {
            LI.sval = 0;
        }
    }

    LI.loc = LI.token.getLocation();
    LI.endloc = LI.token.getLocation().getFileLocWithOffset(LI.token.getLength() - 1);
}

// ---------------------- ParseTree --------------------
ParseTreeAndTokens::ParseTreeAndTokens(Preprocessor& PP, SemanticValue &top,
                                       StringTable &extTable, char const *fname)
  : treeTop(top),
    // RICH: lexer(new OLexer(extTable, PP.getLangOptions(), fname)),
    lexer(new PPLexer(PP, extTable)),
    userAct(NULL),
    tables(NULL),
    PP(PP)
{
}

ParseTreeAndTokens::~ParseTreeAndTokens()
{
  delete lexer;
}


// ---------------------- other support funcs ------------------
// process the input file, and yield a parse graph
bool glrParseNamedFile(GLR &glr, LexerInterface &lexer, SemanticValue &treeTop,
                       char const *inputFname)
{
  #if 0    // old
  // do first phase lexer
  traceProgress() << "lexical analysis...\n";
  traceProgress(2) << "lexical analysis stage 1...\n";
  Lexer1 lexer1(inputFname);
  {
    FILE *input = fopen(inputFname, "r");
    if (!input) {
      xsyserror("fopen", inputFname);
    }

    lexer1_lex(lexer1, input);
    fclose(input);

    if (lexer1.errors > 0) {
      printf("L1: %d error(s)\n", lexer1.errors);
      return false;
    }
  }

  // do second phase lexer
  traceProgress(2) << "lexical analysis stage 2...\n";
  lexer2_lex(lexer2, lexer1, inputFname);

  // parsing itself
  lexer2.beginReading();
  return glr.glrParse(lexer2, treeTop);
  #endif // 0

  PRETEND_USED(inputFname);     // stupid module..
  return glr.glrParse(lexer, treeTop);
}

class EGLR : public GLR {
    Diagnostic& diag;
public:
    EGLR(UserActions *userAct, ParseTables *tables, Diagnostic& diag)
        : GLR(userAct, tables), diag(diag)
        { }
    void parseError(SourceLocation loc, sm::string tokenDesc, int state)
      {
          SourceManager SM;
          diag.Report(FullSourceLoc(loc, SM), diag::err_parse_error)
            << state << tokenDesc.c_str();
      }
    void parseInfo(SourceLocation loc, sm::string tokenInfo, int)
      {
          SourceManager SM;
          diag.Report(FullSourceLoc(loc, SM), diag::note_parse_error_token_info)
            << tokenInfo.c_str();
      }
};

bool toplevelParse(ParseTreeAndTokens &ptree, char const *inputFname)
{
  // parse
  xassert(ptree.userAct != NULL);    // must have been set by now
  xassert(ptree.tables != NULL);

  EGLR glr(ptree.userAct, ptree.tables, ptree.PP.getDiagnostics());

  // parse input
  return glrParseNamedFile(glr, *ptree.lexer, ptree.treeTop, inputFname);
}


// hack: need classifier to act like the one for Bison
class SimpleActions : public TrivialUserActions {
public:
  virtual ReclassifyFunc getReclassifier();
  static int reclassifyToken(UserActions *ths,
    int oldTokenType, SemanticValue sval);
};

UserActions::ReclassifyFunc SimpleActions::getReclassifier()
{
  if (tracingSys("identityReclassify")) {
    // don't reclassify anything
    return &TrivialUserActions::reclassifyToken;
  }
  else {
    // reclassify as if typedef's weren't possible
    return &SimpleActions::reclassifyToken;
  }
}

STATICDEF int SimpleActions::reclassifyToken(UserActions *, int type, SemanticValue)
{
  return type;
}


char *processArgs(int argc, char **argv, char const *additionalInfo) {
  // remember program name
  char const *progName = argv[0];

  // process args
  while (argc >= 2) {
    if (traceProcessArg(argc, argv)) {
      continue;
    }
#if 0
    else if (streq(argv[1], "-sym") && argc >= 3) {
      symOfInterestName = argv[2];
      argc -= 2;
      argv += 2;
    }   
#endif // 0
    else {
      break;     // didn't find any more options
    }
  }

  if (argc != 2) {
    cout << "usage: " << progName << " [options] input-file\n"
//            "  env:\n"
//            "    SYM_OF_INTEREST symbol to watch during analysis\n"
            "  options:\n"
            "    -tr <flags>:       turn on given tracing flags (comma-separated)\n"
            //"    -sym <sym>: name the \"symbol of interest\"\n"
//              "  useful tracing flags:\n"
//              "    parse           print shift/reduce steps of parsing algorithm\n"
//              "    grammar         echo the grammar\n"
//              "    ambiguities     print ambiguities encountered during parsing\n"
//              "    conflict        SLR(1) shift/reduce conflicts (fork points)\n"
//              "    itemsets        print the sets-of-items DFA\n"
//            "    ... the complete list is in parsgen.txt ...\n"
         << (additionalInfo? additionalInfo : "");
    exit(argc==1? 0 : 2);    // error if any args supplied
  }

  return argv[1];
}

void maybeUseTrivialActions(ParseTreeAndTokens &ptree) 
{
  if (tracingSys("trivialActions")) {
    // replace current actions with trivial actions
    //delete ptree.userAct;      // the caller does this
    ptree.userAct = new SimpleActions;
    cout << "using trivial (er, simple..) actions\n";
  }
}

// useful for simple treewalkers
bool treeMain(ParseTreeAndTokens &ptree, int argc, char **argv,
              char const *additionalInfo)
{
  char const *positionalArg = processArgs(argc, argv, additionalInfo);
  maybeUseTrivialActions(ptree);
  return toplevelParse(ptree, positionalArg);
}
