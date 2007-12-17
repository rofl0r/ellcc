/*
 *    Ellp.h - The pre-processing object.
 *
 *    Copyright (C) 2007, Richard Pennington.
 */

#ifndef Ellp_h
#define Ellp_h

#include "pwOS.h"
#include "pwError.h"
#include "pwToken.h"
#include "pwArray.h"
#include "pwStateMachine.h"
#include "pwNode.h"

struct pwWordAssoc {                            // Word/token association.
    char *word;
    // pwString word;                           // RICH: when strings used.
    int token;
};

struct pwBracket {                              // Start/end bracketed definition.
    char *start;
    char *end;
    int token;
};

struct EllpOptions {                            // Pre-processor options.
    bool trigraphs;                             // True if trigraphs enabled.
    int INTEGER;                                // Integer token.
    int CHARACTER;                              // Character token.
    int FLOAT;                                  // Float token.
    int STRING;                                 // String token.
    int IDENTIFIER;                             // Identifier token.
    pwStateMachine *reservedwords;              // Reserved word matcher.
    pwStateMachine *tokens;                     // Token matcher.
    pwBracket *comments;                        // Comment matcher.
};

struct pwTokenInfo : public pwToken {           // Token returned by Ellp.
    enum TokenClass {                           // Classes of tokens.
        TCNONE, TCOPERATOR, TCRESERVED, TCCONSTANT, TCSTRING, TCIDENTIFIER,
        TCPPDIRECTIVE, TCSPACE, TCSKIPPED, TCERROR,
        TCCOUNT
    };

    TokenClass tokenclass;                      // Token's class.
    int token;                                  // Token identifier.
};

class Ellp;                                    // Forward declaration.

class EllpStream : public pwTokenInfo {         // The pre-processing stream object.
public:
    friend class Ellp;
    EllpStream(Ellp& psp, pwPPOptions* options);
    ~EllpStream();
    void setInput(int (Ellp::*getc)())         // Set the input function.
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
    { "<EOF>", EllpStream::ENDOFFILE },   \
    { "NONE", EllpStream::NONE },         \
    { "PINCLUDE", EllpStream::PINCLUDE }, \
    { "PPRAGMA", EllpStream::PPRAGMA },   \
    { "PLINE", EllpStream::PLINE },       \
    { "PDEFINE", EllpStream::PDEFINE },   \
    { "POTHER", EllpStream::POTHER },     \
    { "HEADER", EllpStream::HEADER },     \
    { "NL", EllpStream::NL },             \
    { "WS", EllpStream::WS },             \
    { "COMMENT", EllpStream::COMMENT },

    // Special macro chars.
    enum {
        NEVEREXPAND = 1, STARTMACRO, ENDMACRO, STRINGIZE, PASTE
    };

    void getToken(pwTokenInfo& data);           // Get the next token from a stream.
    void getToken();                            // Get the next token from a stream.
    void optionsChanged();                      // Notify stream of changed options.
    int startLine()                             // Get the current tokens starting line.
        { return startline; }

private:
    pwString type;                              // Type of macro.
    bool funlike;                               // Is this a function-like macro?
    pwArray<pwString> formal;                   // Names of the formal arguments.
    pwString body;                              // The macro body.

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
        Stream(Stream *next, const pwString& name, const pwString& body, int nextchar, bool inhibit);
        Stream *next;                           // Next stream.
        bool inhibit;                           // Inhibit name scan.
        pwString name;                          // Name associated with stream.
        pwString body;                          // Body of macro.
        int index;                              // For sequencing through the body.
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
    EllpOptions *options;                       // Pre-processor options.
    Stream *first;                              // First open stream.
    int line, column;                           // Current source position.
    int commentstartline, commentstartcolumn;
    int commentendline, commentendcolumn;
    char *commentbuffer;                        // Start of comment buffer.

    int (Ellp::*fgetc)();                      // Input function.
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

    Ellp& psp;                                 // Preprocessor context.

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
    static char *convertcharacter(long *value, const pwString& string);
    static char *convertnumber(long *value, const pwString& string);
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

    void escape(pwString& string);
    PPDirectives isppdirective();
    int isreserved();
    void readchar();
    void getnextchar();

    static const pwWordAssoc directivelist[];
    static const pwWordAssoc operatorlist[];
};

class Ellp {                                   // The pre-processor object.
public:
    enum Filter {                               // Token filters.
        GETALL,                                 // Get all tokens.
        GETNWS,                                 // Get all non-whitespace tokens.
        GETNL,                                  // Get all non-whitespace tokens plus newline.
    };

    Ellp(const pwString& name, pwNodePtr& macroTable);
    bool setInput(const char *string);
    bool setInput(FILE *fp = NULL);
    void addInclude(const pwString& name);
    void addDefine(const pwString& name, const pwString& value);
    void undefine(const pwString& name, bool fixed);
    void fixedDefine(const pwString& name, const char *value);
    virtual ~Ellp();
    void getOptions(EllpOptions *op);
    void setOptions(EllpOptions *op);
    void getToken(pwTokenInfo& info, Filter filter);
    const pwArray<pwString>& depends();
    int isdefined(const pwString& name, int line);
    bool lookupmacro(const pwString& name, int line, pwNodePtr& mpp);
    virtual pwError* error(pwError::Type, int, int, int, int, const char*, ...) = 0;
    virtual void errorPosition(pwString&, const pwString&, int, int, int, int, bool) = 0;
private:
    struct include {                            // Include file definition.
        include *next;                          // Next in include list.
        pwString name;                          // Name of include file.
        FILE *fp;                               // Include file.
        EllpStream *pp;                         // Scanner context.
    };

    pwString name;                              // Name of input.
    bool myfp;                                  // True if fp is internal.
    FILE *fp;                                   // File pointer.
    char *sp;                                   // String pointer.
    char *ip;                                   // string input pointer
    EllpStream *pp;                             // Scanner context.
    include *includes;                          // open include files
    int includeline;                            // last #include line
    pwArray<pwString> files;                    // Input file names.
    pwArray<pwString> includedirs;              // Include search path.
    EllpOptions options;                        // Pre-processor options.
    pwNodePtr& macros;                          // The macro table.
    pwNodePtr lookup(const pwString& name, int line);
    void definemacro(int line,const pwString& filename, EllpStream* data);
    void definemacro(int line,const pwString& filename, const pwTokenInfo& data,
                     const pwString& type, bool funlike, const pwArray<pwString>& formal, const pwString& body);
    void undefinemacro(const pwString& name, int line, int fileline, bool fixed);
    int stringgetc();
    int filegetc();
    pwString addname(const pwString& name);
    void initializeoptions();
    void processnexttoken(pwTokenInfo& tinfo);
    bool process();
};

#endif
