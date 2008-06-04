/*
 *    pwPP.h - The pre-processing object.
 *
 *    Copyright (C) 2008, Richard Pennington.
 */

#ifndef pwPP_h
#define pwPP_h

#include <string>
#include <map>
#include "pwArray.h"
#include "pwTable.h"
#include "pwError.h"
#include "pwMatcher.h"

namespace pw {

struct WordAssoc {                            // Word/token association.
    char *word;
    int token;
};

struct Bracket {                              // Start/end bracketed definition.
    char *start;
    char *end;
    int token;
};

struct Options {                            // Pre-processor options.
    bool trigraphs;                             // True if trigraphs enabled.
    int INTEGER;                                // Integer token.
    int CHARACTER;                              // Character token.
    int FLOAT;                                  // Float token.
    int STRING;                                 // String token.
    int IDENTIFIER;                             // Identifier token.
    pw::Matcher *reservedwords;              // Reserved word matcher.
    pw::Matcher *tokens;                     // Token matcher.
    const Bracket *comments;                        // Comment matcher.
    Options(bool trigraphs = false, int INTEGER = 0, int CHARACTER = 0, int FLOAT = 0,
            int STRING = 0, int IDENTIFIER = 0, pw::Matcher *reservedwords = NULL,
            pw::Matcher *tokens = NULL, const Bracket *comments = NULL)
        : trigraphs(trigraphs), INTEGER(INTEGER), CHARACTER(CHARACTER), FLOAT(FLOAT),
          STRING(STRING), IDENTIFIER(IDENTIFIER), reservedwords(reservedwords),
          tokens(tokens), comments(comments) { }
};

struct Position {				// An input stream position.
    Position()
        { startline = 0; startcolumn = 0; endline = 0; endcolumn = 0; }
    Position(const Position& value)
        {
            startline = value.startline; startcolumn = value.startcolumn;
            endline = value.endline; endcolumn = value.endcolumn;
            file = value.file;
        }
    int startline, startcolumn;                 // Position of token in input stream.
    int endline, endcolumn;
    std::string file;                              // Name of input stream.
};

struct Token : public Position {            // A preprocessor token.
    Token() : Position() {}
    Token(const Token& value) : Position(value)
        {
            string = value.string;
        }
    std::string string;                            // Token string.
};

struct TokenInfo : public Token {           // Token returned by .
    enum TokenClass {                           // Classes of tokens.
        TCNONE, TCOPERATOR, TCRESERVED, TCCONSTANT, TCSTRING, TCIDENTIFIER,
        TCPPDIRECTIVE, TCSPACE, TCSKIPPED, TCERROR,
        TCCOUNT
    };

    TokenClass tokenclass;                      // Token's class.
    int token;                                  // Token identifier.
};

struct Macro : public Token
{
public:
    Macro() : function(false), undefined(0) {}
    std::string body;				// Macro body.
    bool function;				// If this a function-like macro?
    int undefined;
    std::string type;
    pw::array<std::string> arguments;		// Macro arguments.
    std::string& name() { return string; }
};

typedef Table<Macro*> MacroTable;

class PP;                                    // Forward declaration.

class PPStream : public TokenInfo {         // The pre-processing stream object.
public:
    friend class PP;
    PPStream(PP& psp, Options* options);
    ~PPStream();
    void setInput(int (PP::*getc)())         // Set the input function.
        { fgetc = getc; }

    // Token definitions.
    enum {
        ENDOFFILE, NONE,
    
        PINCLUDE, PPRAGMA, PLINE, PUNDEF, PDEFINE, POTHER, HEADER,
        NL, WS, COMMENT,
    
        CTNEXTOKEN                              // This must be defined last.
    };

// Strings and values representing the basic pscan tokens.
#define pwPSCANTOKENS                     \
    { "<EOF>", PPStream::ENDOFFILE },   \
    { "NONE", PPStream::NONE },         \
    { "PINCLUDE", PPStream::PINCLUDE }, \
    { "PPRAGMA", PPStream::PPRAGMA },   \
    { "PLINE", PPStream::PLINE },       \
    { "PDEFINE", PPStream::PDEFINE },   \
    { "POTHER", PPStream::POTHER },     \
    { "HEADER", PPStream::HEADER },     \
    { "NL", PPStream::NL },             \
    { "WS", PPStream::WS },             \
    { "COMMENT", PPStream::COMMENT },

    // Special macro chars.
    enum {
        NEVEREXPAND = 1, STARTMACRO, ENDMACRO, STRINGIZE, PASTE
    };

    void getToken(TokenInfo& data);           // Get the next token from a stream.
    void getToken();                            // Get the next token from a stream.
    void optionsChanged();                      // Notify stream of changed options.
    int startLine()                             // Get the current tokens starting line.
        { return startline; }

private:
    std::string type;                              // Type of macro.
    bool funlike;                               // Is this a function-like macro?
    pw::array<std::string> formal;                   // Names of the formal arguments.
    std::string body;                              // The macro body.

    struct Conditional {                        // Conditional compilation control.
        Conditional *next;                      // Next active conditional.
        bool skipping;                          // Is enclosing conditional skipping?
        bool haselse;                           // This conditional has an else.
        bool hastruepart;                       // This conditional has a true part.
        int line, column;                       // Starting line, column.
        int skipline, skipcolumn;               // Start of skipped source.
    };

    class Stream {                              // for macro streams
    public:
        Stream(Stream *next, const std::string& name, const std::string& body, int nextchar, bool inhibit);
        Stream *next;                           // Next stream.
        bool inhibit;                           // Inhibit name scan.
        std::string name;                          // Name associated with stream.
        std::string body;                          // Body of macro.
        size_t index;                              // For sequencing through the body.
        int oldnextchar;                        // Last character in old stream.
    };

    enum PPDirectives {                         // Preprocessor directives.
        PPDNONE = -1, PPDEFINE, PPELIF, PPELSE, PPENDIF, PPERROR, PPIF,
        PPIFDEF, PPIFNDEF, PPINCLUDE, PPLINE, PPPRAGMA, PPUNDEF,
    };

    enum {
        POUND, POUNDPOUND, COMMA, LPAREN, RPAREN,
        QUESTION, COLON, TILDA, EQ, STAR, SLASH,
        PERCENT, PLUS, HYPHEN, AND, ANDAND, CAROT,
        BAR, OROR, EXCLAMATION, NE, ELIPSIS,
        LT, LE, LL, GT, GE, RR,
        OPERCOUNT
    };

    bool hasspace;                              // Compress whitespace.
    Options *options;                       // Pre-processor options.
    Stream *first;                              // First open stream.
    int line, column;                           // Current source position.
    int commentstartline, commentstartcolumn;
    int commentendline, commentendcolumn;
    char *commentbuffer;                        // Start of comment buffer.

    int (PP::*fgetc)();                      // Input function.
    bool iscomment;                             // Token is a comment.
    int nextchar;                               // Next character to scan.
    int nextl, nextc;                           // Position of nextchar.

    bool sawnewline;                            // For # directives and asm labels.
    bool skipping;                              // true if skipping input.
    int newlinecount;                           // Count of scanned newlines.
    bool allowheader;                           // Allow preprocessor header name.
    bool neverexpand;                           // Set for IDENTIFIER.
    bool noexpand;                              // Set to inhibit macro expansion.
    bool inpragma;                              // Set if scanning a pragma.

    Conditional *conditionals;                  // Active conditional list.

    PP& psp;                                 // Preprocessor context.

    int maxtoken;                               // Maximum token value.

    // Internal tokens.
    int operators[OPERCOUNT];

    // information about a token
    bool sysheader;                             // true if string is a <> header.
    int linevalue;                              // Line # returned by #line.

    static int read(void *arg);
    static void save(void *arg, int current);
    static void backup(void *arg, int good, int count);
    void setupStateMachines();
    static char *convertcharacter(long *value, const std::string& string);
    static char *convertnumber(long *value, const std::string& string);
    bool primary(long *value);  
    bool unaryexpression(long *value);
    bool multiplicativeexpression(long *value);
    bool additiveexpression(long *value);
    bool shiftexpression(long *value);
    bool relationalexpression(long *value);
    bool equalityexpression(long *value);
    bool ANDexpression(long *value);
    bool exclusiveORexpression(long *value);
    bool inclusiveORexpression(long *value);
    bool logicalANDexpression(long *value);
    bool logicalORexpression(long *value);
    bool conditionalexpression(long *value);
    bool expression(long *value);
    bool conditionalexpr();
    void getpptoken();
    void pptoken();

    void escape(std::string& string);
    PPDirectives isppdirective();
    int isreserved();
    void readchar();
    void getnextchar();

    static const WordAssoc directivelist[];
    static const WordAssoc operatorlist[];
};

class  PP {                                   // The pre-processor object.
public:
    enum Filter {                               // Token filters.
        GETALL,                                 // Get all tokens.
        GETNWS,                                 // Get all non-whitespace tokens.
        GETNL,                                  // Get all non-whitespace tokens plus newline.
    };

    PP(const std::string& name, MacroTable& macroTable);
    virtual ~PP();
    bool setInput(const char *string);
    bool setInput(FILE *fp = NULL);
    void addInclude(const std::string& name);
    void addDefine(const std::string& name, const std::string& value);
    void undefine(std::string& name, bool fixed);
    void fixedDefine(const std::string& name, const char *value);
    void getOptions(Options *op);
    void setOptions(Options *op);
    void getToken(TokenInfo& info, Filter filter);
    const pw::array<std::string>& depends();
    int isdefined(std::string& name, int line);
    bool lookupmacro(std::string& name, int line, Macro*& mpp);
    virtual pw::Error* error(pw::Error::Type, int, int, int, int, const char*, ...) = 0;
    virtual void errorPosition(std::string&, const std::string&, int, int, int, int, bool) = 0;
private:
    struct include {                            // Include file definition.
        include *next;                          // Next in include list.
        std::string name;                          // Name of include file.
        FILE *fp;                               // Include file.
        PPStream *pp;                         // Scanner context.
    };

    std::string name;                              // Name of input.
    bool myfp;                                  // True if fp is internal.
    FILE *fp;                                   // File pointer.
    char *sp;                                   // String pointer.
    char *ip;                                   // string input pointer
    PPStream *pp;                             // Scanner context.
    include *includes;                          // open include files
    int includeline;                            // last #include line
    pw::array<std::string> files;                    // Input file names.
    pw::array<std::string> includedirs;              // Include search path.
    Options options;                        // Pre-processor options.
    MacroTable& macros;                          // The macro table.
    Macro* lookup(std::string& name, int line);
    void definemacro(int line, const std::string& filename, PPStream* data);
    void definemacro(int line, const std::string& filename, TokenInfo& data,
                     const std::string& type, bool funlike, const pw::array<std::string>& formal, const std::string& body);
    void undefinemacro(std::string& name, int line, int fileline, bool fixed);
    int stringgetc();
    int filegetc();
    std::string& addname(const std::string& name);
    void initializeoptions();
    void processnexttoken(TokenInfo& tinfo);
    bool process();
};

};

#endif
