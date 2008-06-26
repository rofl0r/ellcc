/** A lexer with primative parsing hooks.
 */

#ifndef pwPlexer_h
#define pwPlexer_h

#include "pwPP.h"

namespace pw {

/** A lexer with primative parsing hooks.
 */
class Plexer {
public:
    /** Create a Plexer definition.
     * @param name The language name.
     * @return A unique instance for each language or NULL if an error occured.
     */
    static const Plexer* Create(std::string name, ErrorList& error);

private:
    /** Constructor (not defined or used).
     */
    Plexer();
    /** Copy constructor (not defined or used).
     */
    Plexer(Plexer&);
    /** Constructor.
     */
    Plexer(std::string name, ErrorList& errors);
    /** Destructor.
     */
    ~Plexer() { delete options.reservedWords; delete options.tokens; }

public:
    /** The error list.
     */
    ErrorList& errors;

    /** Preprocessor options.
     */
    Options options;
    /** The predefined macro table.
     */
    array<Macro> macros;
    /** The include path table.
     */
    array<std::string> includes;
    /** Configuration file token identifiers.
     */
    enum CFGTokens {
        STRING = PPStream::CTNEXTOKEN, CHARACTER, INTEGER, FLOAT, IDENTIFIER,
        LBRACE, RBRACE, COMMA, ASSIGN, SEMICOLON, RANGE,
    };
    /** A parsing function.
     */
    typedef void Parser(PP& pp, Plexer& env, ErrorList& errors, void* data);
    /** Define a language sentence parser.
     */
    void first(std::string name, Parser* handler);
    /** Parse a language file.
     */
    bool parse(std::string name, void* data, array<Macro>* macros = NULL);

private:
    /** The language name.
     */
    std::string name;
    /** The known language array.
     */
    static array<Plexer*> languages;

    /** Set preprocessor options.
     */
    void CFGsetupOptions();

    /** Default language file options.
     */
    static Options CFGoption;

    /** Configuration file reserved words.
     */
    static const WordAssoc CFGreservedWords[];
    /** Configuration file tokens.
     */
    static const WordAssoc CFGtokens[];
    /** Configuration file token names.
     */
    static const char* CFGtokenName(int value, void* context);
    /** Configuration file comments.
     */
    static Bracket CFGcomments[];

    /* Extensible parser handling.
     */
    /** A Parser entry.
     */
    class Entry {
    public:
        Entry(std::string string, Parser* parser)
            : parser(parser), string(string) { }
        std::string& name() { return string; }
        Parser* parser;
    private:
        Entry();
        std::string string;
    };

    /** The parser table.
     */
    Table<Entry*> parsers;

    struct TokenDefinition : Token {                // A token definition.
        TokenDefinition(const Token& value)
            : Token(value), value(0), keyword(false)
            { }

        std::string& name()                         // Return the name of this token.
            { return string; }

        array<Token*> regex;                        // Defining regular expression(s), if any.
        Matcher::Input value;                       // Token value.
        bool keyword;                               // True if Token is a keyword.
    };

    struct State {
        State()
            : tokenCount(PPStream::CTNEXTOKEN), keywordCount(0), nextToken(PPStream::CTNEXTOKEN)
            { }
        Table<TokenDefinition*> tokenTable;            	// Token hash table.
        int tokenCount;                             	// Number of tokens.
        int keywordCount;                           	// Number of keywords.
        Matcher::Input nextToken;                   	// Next token number.
        Plexer* language;                             	// The language to build.
    };

    static TokenDefinition* newToken(Token& info, State* state, bool keyword);

    /* Set up token information.
     */
    void setupTokens(State* sp);
    
    /* Configuration language sentence handlers.
     */
    /** Parse tokens and keywords.
     */
    static Parser parseTokens;
    /** Parse comments.
     */
    static Parser parseComments;
    /** Parse include directories.
     */
    static Parser parseIncludes;
    /** Set the needwhitespace flag.
     */
    static Parser parseNeedwhitespace;

    /** Reserved words defined in this language.
     */
    WordAssoc* reservedWords;
    /** Tokens words defined in this language.
     */
    WordAssoc* tokens;
};

};

#endif
