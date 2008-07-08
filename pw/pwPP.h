/** @file
 * The pw preprocessor.
 * @author Richard Pennington
 * @date June 29, 2008
 *
 * Copyright (C) 2008, Richard Pennington.
 */

#ifndef pwPP_h
#define pwPP_h

#include <string>
#include "pwArray.h"
#include "pwTable.h"
#include "pwError.h"
#include "pwMatcher.h"

namespace pw {

/** Associate a string with a value.
 */
struct WordAssoc {
    const char *word;                           ///< The string.
    int token;                                  ///< The associated value.
};

/** A comment bracket definition and the associated value.
 */
struct Bracket {
    const char *start;                          ///< The start string of a comment.
    const char *end;                            ///< The end string of a comment.
    int token;                                  ///< The associated value.
};

/** An input stream position.
 */
struct Position {
    /** The default constructor.
     */
    Position()
        { startline = 0; startcolumn = 0; endline = 0; endcolumn = 0; }
    /** The copy constructor.
     */
    Position(const Position& value)
        {
            startline = value.startline; startcolumn = value.startcolumn;
            endline = value.endline; endcolumn = value.endcolumn;
            file = value.file;
        }

    int startline;                              ///< The starting line.
    int startcolumn;                            ///< The starting column.
    int endline;                                ///< The ending line.
    int endcolumn;                              ///< The ending column.
    const char* file;                           ///< The name of the input stream.
};

/** A token has a string and a source position associeted with it.
 */
struct Token : public Position {
    /** The default constructor.
     */
    Token() : Position() {}
    /** The copy constructor.
     */
    Token(const Token& value) : Position(value)
        {
            string = value.string;
        }
    std::string string;                         ///< The string associated with the token.
};

/** A classified token with a value.
 */
struct TokenInfo : public Token {
    /** Classes of tokens.
     */
    enum TokenClass {
        TCNONE,                                 ///< No classification.
        TCOPERATOR,                             ///< An operator.
        TCRESERVED,                             ///< A reserved word.
        TCCONSTANT,                             ///< A constant.
        TCSTRING,                               ///< A string.
        TCIDENTIFIER,                           ///< An identifier
        TCPPDIRECTIVE,                          ///< A preprocessing directive.
        TCSPACE,                                ///< Whitespace.
        TCSKIPPED,                              ///< A skipped token (e.g. via \#if).
        TCERROR,                                ///< An error.
        TCCOUNT                                 ///< The number of token classes.
    };

    TokenClass tokenclass;                      ///< The token's class.
    int token;                                  ///< The token's identifier.
};

/** A macro definition.
 */
struct Macro : public Token
{
public:
    /** The macro type.
     */
    enum Type {
        DEFINED_MACRO,                          ///< A user defined macro.
        FIXED_MACRO,                            ///< A fixed macro.
        FILE_MACRO,                             ///< __FILE__.
        LINE_MACRO,                             ///< __LINE__.
    };

    /** The default constructor.
     */
    Macro() : function(false), undefined(0), type(DEFINED_MACRO)  {}

    std::string body;				///< The macro body.
    bool function;				///< Is this a function-like macro?
    int undefined;                              ///< The source line number where this macro was \#undef'd.
    Type type;                                  ///< The macro type.
    array<std::string> arguments;		///< The formal arguments.
    /** Get the name of a macro.
     * @return The macro's name.
     */
    std::string& name() { return string; }
};

/** A macro lookup table.
 */
typedef Table<Macro*> MacroTable;

// Forward declarations.
class PP;
class Options;

/** A preprocessing stream.
 */
class PPStream : public TokenInfo {
public:
    friend class PP;

    /** The constructor.
     * @param psp The preprocessing context.
     * @param options Preprocessor options in this context.
     */
    PPStream(PP& psp, Options* options);
    /** The destructor.
     */
    ~PPStream();

    /** Set the input function for the stream.
     * @param getc The function to retrurn a character from the stream.
     */
    void setInput(int (PP::*getc)())
        { fgetc = getc; }

    /** Token definitions.
     * These are the set of tokens that must be known by the preprocessor.
     */
    enum {
        ENDOFFILE,                              ///< End of file.
        NONE,                                   ///< Not a defined token.
        PINCLUDE,                               ///< An include directive.
        PPRAGMA,                                ///< A pragma.
        PLINE,                                  ///< A line directive.
        PUNDEF,                                 ///< An undef directive.
        PDEFINE,                                ///< A define directive.
        POTHER,                                 ///< Some other preprocessing directive.
        HEADER,                                 ///< An include file name.
        NL,                                     ///< A newline.
        WS,                                     ///< Witespace.
        COMMENT,                                ///< A comment.
    
        CTNEXTOKEN                              ///< The number of preprocessor tokens. This must be defined last.
    };

/** Strings and values representing the basic pscan tokens.
 */
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

    /** Special macro characters.
     */
    enum {
        NEVEREXPAND = 1,                        ///< Never expand the identifier.
        STARTMACRO,                             ///< The start of a macro body.
        ENDMACRO,                               ///< The end of a macro body.
        STRINGIZE,                              ///< Stringize the argument.
        PASTE                                   ///< Paste tokens.
    };

    /** Get the next token from a stream.
     * @param data The returned token's information.
     */
    void getToken(TokenInfo& data);
    /** Get the next token from a stream.
     */
    void getToken();
    /** Notify the stream of changed options.
     */
    void optionsChanged(); 

    /** Get the current token's starting line.
     * @return The starting line position.
     */
    int startLine()
        { return startline; }

private:
    Macro::Type type;                           ///< The type of the current macro.
    bool funlike;                               ///< Is this a function-like macro?
    array<std::string> formal;                  ///< Names of the formal arguments.
    std::string body;                           ///< The macro body.

    /** Conditional compilation control.
     */
    struct Conditional {
        Conditional *next;                      ///< The next active conditional.
        bool skipping;                          ///< Is the enclosing conditional skipping?
        bool haselse;                           ///< This conditional has an else.
        bool hastruepart;                       ///< This conditional has a true part.
        int line, column;                       ///< Starting line, column.
        int skipline, skipcolumn;               ///< Start of skipped source.
    };

    /** A macro stream.
     */
    class Stream {
    public:
        /** The constructor.
         * @param next The enclosing stream.
         * @param name The name of the stream (i.e. the macro name).
         * @param body The stream.
         * @param nextchar The cjaracter immediately following the stream.
         * @param inhibit true if name scanning is inhibited in this stream.
         */
        Stream(Stream *next, const std::string& name, const std::string& body, int nextchar, bool inhibit);
        Stream *next;                           ///< Next stream.
        bool inhibit;                           ///< Inhibit name scan.
        std::string name;                       ///< Name associated with stream.
        std::string body;                       ///< Body of macro.
        size_t index;                           ///< For sequencing through the body.
        int oldnextchar;                        ///< Last character in old stream.
    };

    /** Preprocessor directives.
     */
    enum PPDirectives {
        PPDNONE = -1,                           ///< No directive.
        PPDEFINE,                               ///< \#define
        PPELIF,                                 ///< \#elif
        PPELSE,                                 ///< \#else
        PPENDIF,                                ///< \#endif
        PPERROR,                                ///< \#error
        PPIF,                                   ///< \#if
        PPIFDEF,                                ///< \#ifdef
        PPIFNDEF,                               ///< \#ifndef
        PPINCLUDE,                              ///< \#include
        PPINCLUDE_NEXT,                         ///< \#include_next
        PPLINE,                                 ///< \#line
        PPPRAGMA,                               ///< \#pragma
        PPUNDEF,                                ///< \#undef
    };

    /** Tokens used to parse preprocessor directives.
     */
    enum {
        POUND,                                  ///< \#
        POUNDPOUND,                             ///< \#\#
        COMMA,                                  ///< ,
        LPAREN,                                 ///< (
        RPAREN,                                 ///< )
        QUESTION,                               ///< ?
        COLON,                                  ///< :
        TILDA,                                  ///< ~
        EQ,                                     ///< ==
        STAR,                                   ///< *
        SLASH,                                  ///< /
        PERCENT,                                ///< %
        PLUS,                                   ///< +
        HYPHEN,                                 ///< -
        AND,                                    ///< &
        ANDAND,                                 ///< &&
        CAROT,                                  ///< ^
        BAR,                                    ///< |
        OROR,                                   ///< ||
        EXCLAMATION,                            ///< !
        NE,                                     ///< !=
        ELIPSIS,                                ///< ...
        LT,                                     ///< <
        LE,                                     ///< <=
        LL,                                     ///< <<
        GT,                                     ///< >
        GE,                                     ///< >=
        RR,                                     ///< >>
        OPERCOUNT                               ///< The number of preprocessor operators.
    };

    bool hasspace;                              ///< Whitespace has been encountered. Used to compress whitespace.
    Options *options;                           ///< Pre-processor options.
    Stream *first;                              ///< First open stream.
    int line, column;                           ///< The current source position.
    int commentstartline;                       ///< The starting line of the current comment.
    int commentstartcolumn;                     ///< The starting column of the current comment.
    int commentendline;                         ///< The ending line of the current comment.
    int commentendcolumn;                       ///< The ending column of the current comment.
    char *commentbuffer;                        ///< The comment start string buffer.

    int (PP::*fgetc)();                         ///< The input function.
    bool iscomment;                             ///< The current token is a comment.
    int nextchar;                               ///< The next character to scan.
    int nextl;                                  ///< The line number of nextchar.
    int nextc;                                  ///< The column of nextchar.

    bool sawnewline;                            ///< For # directives and asm labels.
    bool skipping;                              ///< true if skipping input.
    int newlinecount;                           ///< The count of scanned newlines.
    bool allowheader;                           ///< Gather a preprocessor header name.
    bool neverexpand;                           ///< Set for IDENTIFIER.
    bool noexpand;                              ///< Set to inhibit macro expansion.
    bool inpragma;                              ///< Set if scanning a pragma.
    bool inppdirective;                         ///< Set if scanning a pre-processor directive.
    Conditional *conditionals;                  ///< The active conditional list.
    PP& psp;                                 	///< The preprocessor context.
    int maxtoken;                               ///< The maximum token value.
    int operators[OPERCOUNT];                   ///< Internal token values.

    // information about a token
    bool sysheader;                             ///< true if string is a <> header.
    bool include_next;                          ///< true if \#include_next.
    int linevalue;                              ///< Line \# returned by \#line.

    /** Get the next character.
     * A Matcher::matchStream callback function.
     * @param arg The context.
     * @return The next character in the stream.
     */
    static int read(void *arg);
    /** Save the next character.
     * A Matcher::matchStream callback function.
     * @param arg The context.
     * @param current The character to save.
     */
    static void save(void *arg, int current);
    /** Back up N characters in a stream.
     * A Matcher::matchStream callback function.
     * @param arg The context.
     * @param good The number of good characters.
     * @param count The number of characters to back up.
     */
    static void backup(void *arg, int good, int count);
    /** Set to the lexer state machines.
     */
    void setupStateMachines();
    typedef long long ppint_t;                  ///< The type of a preprocessor expression integer.
    /** Convert a string representing a character constant to a value.
     * @param[out] value The resulting value.
     * @param string The string to convert.
     * @return If non-NULL, an error string.
     */
    static char *convertCharacter(ppint_t *value, const std::string& string);
    /** Convert a string representing a numeric constant to a value.
     * @param[out] value The resulting value.
     * @param string The string to convert.
     * @return If non-NULL, an error string.
     */
    static const char *convertNumber(ppint_t *value, const std::string& string);
    /** Parse a primary expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool primary(ppint_t *value);  
    /** Parse a unary expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool unaryexpression(ppint_t *value);
    /** Parse a multiplicative expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool multiplicativeexpression(ppint_t *value);
    /** Parse an additive expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool additiveexpression(ppint_t *value);
    /** Parse a shift expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool shiftexpression(ppint_t *value);
    /** Parse a relational expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool relationalexpression(ppint_t *value);
    /** Parse an equality expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool equalityexpression(ppint_t *value);
    /** Parse an AND expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool ANDexpression(ppint_t *value);
    /** Parse an exclusive OR expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool exclusiveORexpression(ppint_t *value);
    /** Parse an inclusive expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool inclusiveORexpression(ppint_t *value);
    /** Parse a logical AND expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool logicalANDexpression(ppint_t *value);
    /** Parse a logical OR expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool logicalORexpression(ppint_t *value);
    /** Parse a conditional expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool conditionalexpression(ppint_t *value);
    /** Parse a preprocessor expression.
     * One of the preprocessor expression parsing functions.
     * @param[in,out] value The value of the preprocessor expression.
     * @return true if the parse is valid.
     */
    bool expression(ppint_t *value);
    /** Parse a conditional expression.
     * One of the preprocessor expression parsing functions.
     * @return true if the parse results in a non-zero value.
     */
    bool conditionalexpr();
    void getpptoken();                          ///< Get the next non-blank preprocessor token.
    void pptoken();                             ///< Get the next preprocessor token.
    /** Gather an escape sequence.
     * @param[out] string The escape sequence.
     */
    void escape(std::string& string);
    /** Determine whether the current string is a preprocessor directive.
     * @return #PPDNONE if the string is not a directive otherwise a valid #PPDirectives.
     */
    PPDirectives isppdirective();
    /** Is the current string a reserved word?
     * @return -1 if not, otherwise the reserved word token value.
     */
    int isreserved();
    void readchar();                            ///< Get the next character from the input stream.
    void getnextchar();                         ///< The low level character reader.
    static const WordAssoc directivelist[];     ///< The preprocessor directive list.
    static const WordAssoc operatorlist[];      ///< The preprocessor operator list.
};

/** Pre-processor options.
 */
struct Options {
    bool trigraphs;                             ///< True if trigraphs enabled.
    int INTEGER;                                ///< Integer token.
    int CHARACTER;                              ///< Character token.
    int FLOAT;                                  ///< Float token.
    int STRING;                                 ///< String token.
    int IDENTIFIER;                             ///< Identifier token.
    Matcher* reservedWords;                     ///< Reserved word matcher.
    Matcher* tokens;                     	///< Token matcher.
    Bracket* comments;                          ///< Comment matcher.
    /** The Option constructor.
     */
    Options(bool trigraphs = false, int INTEGER = PPStream::NONE,
            int CHARACTER = PPStream::NONE, int FLOAT = PPStream::NONE,
            int STRING = PPStream::NONE, int IDENTIFIER = PPStream::NONE,
            Matcher* reservedWords = NULL, Matcher* tokens = NULL, Bracket* comments = NULL)
        : trigraphs(trigraphs), INTEGER(INTEGER), CHARACTER(CHARACTER), FLOAT(FLOAT),
          STRING(STRING), IDENTIFIER(IDENTIFIER), reservedWords(reservedWords),
          tokens(tokens), comments(comments) { }
};

/** A preprocessor.
 */
class  PP {
public:
    /** Token filters.
     */
    enum Filter {
        GETALL,                                 ///< Get all tokens.
        GETNWS,                                 ///< Get all non-whitespace tokens.
        GETNL,                                  ///< Get all non-whitespace tokens plus newline.
    };

    /** The preprocessor constructor.
     * @param name The main file name.
     * @param errors The error list.
     */
    PP(const std::string& name, ErrorList& errors);
    /** The preprocessor destructor.
     */
    ~PP();
    TokenInfo info;                             ///< Information about he current token.
    /** Set the current input stream to a string.
     * @param string The string to preprocess.
     * @return true if the operation suceedded.
     */
    bool setInput(const char *string);
    /** Set the current input stream to a file.
     * @param fp If non-NULL The input file pointer, if NULL the current #name is opened..
     * @return true if the operation suceedded.
     */
    bool setInput(FILE *fp = NULL);
    void addUserInclude(const std::string& name);
    void addInclude(const std::string& name);
    void addDefine(const std::string& name, const std::string& value = "");
    void addDefine(Macro& macro);
    void undefine(std::string& name, bool fixed);
    void fixedDefine(const std::string& name, const char *value);
    void getOptions(Options *op);
    void setOptions(Options *op);
    void getToken(Filter filter = PP::GETNWS);
    const array<const char*>& depends();
    int isdefined(std::string& name, int line);
    bool lookupmacro(std::string& name, int line, Macro*& mpp);
    ErrorList& errors;
    Error* error(Error::Type type, int sl, int sc, int el, int ec, const char* string, ...);
    void errorPosition(std::string& buffer, const char* file, int sl, int sc, int el, int ec, bool trailer)
        { errors.position(buffer, file, sl, sc, el, ec, trailer); }

    /** The macro table.
     */
    MacroTable macros;
private:
    /** An include file definition.
     */
    struct include {
        include *next;                          ///< Next in the include stack.
        const char* name;                       ///< The name of the include file.
        FILE *fp;                               ///< The include file.
        PPStream *pp;                           ///< The scanner context.
        int level;                              ///< The current file's include path level.
        bool user;                              ///< true if level is in the user include path.
    };

    const char* name;                           ///< The name of input stream.
    bool myfp;                                  ///< True if fp is internal.
    FILE *fp;                                   ///< File pointer, if stream is a file.
    char *sp;                                   ///< String pointer, if stream is a string.
    char *ip;                                   ///< string input pointer
    PPStream *pp;                               ///< The Stream context.
    include *includes;                          ///< The open include list.
    int includeline;                            ///< last \#include line
    static array<const char*> files;            ///< Input file names.
    array<std::string> userincludedirs;         ///< The user include search path.
    array<std::string> includedirs;             ///< The include search path.
    Options options;                            ///< The preprocessor options.
    /** Look up a macro.
     * @param name The macro name to look up.
     * @param line The current source line number.
     * @return A pointer to the macro definition or NULL if it is not found.
     */
    Macro* lookup(std::string& name, int line);
    /** Define a macro with the current stream information.
     * @param line The source line where the definition occurs.
     * @param filename The name of the source file.
     * @param data The input stream containing the macro definition.
     */
    void definemacro(int line, const char* filename, PPStream* data);
    /** Define a macro.
     * @param line The source line where the definition occurs.
     * @param filename The name of the source file.
     * @param data The token containing the macro name.
     * @param type The type of the macro.
     * @param funlike true if the macro is a function-like macro.
     * @param formal The macro's formal arguments if the macro is a function-like macro.
     * @param body The body of the macro.
     */
    void definemacro(int line, const char* filename, Token& data,
                     Macro::Type type, bool funlike, const array<std::string>& formal, const std::string& body);
    /** Undefine a macro.
     * @param name The name of the macro.
     * @param line The current source line.
     * @param fileline The line at which the current file was included.
     * @param fixed true if the macro can't be changed.
     */
    void undefinemacro(std::string& name, int line, int fileline, bool fixed);
    /** Low level function to get the next character from a string.
     * @return The next character in the string or -1 if the end of the string is encountered.
     */
    int stringgetc();
    /** Low level function to get the next character from a file.
     * @return The next character in the file or -1 if the end of the file is encountered.
     */
    int filegetc();
    /** Add a file name to the file list.
     * @param name The name to add.
     * @return A pointer to the added name.
     */
    const char* addName(const char* name);
    void initializeoptions();                   ///< Set preprocessor options to the default state.
    /** Open an include file.
     * @param current The current preprocessing context.
     * @return true if the file was opened successfully.
     */
    bool doInclude(PPStream *current);
    /** Get the net token, handling some preprocessor directives.
     * @param[out] tinfo Information about the next token.
     */
    void processnexttoken(TokenInfo& tinfo);
    void process();                             ///< Process a file.
};

};

#endif
