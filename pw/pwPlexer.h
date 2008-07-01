/** @file
 * A lexer with primative parsing hooks.
 * @author Richard Pennington
 * @date July 1, 2008
 *
 * Copyright (C) 2008, Richard Pennington.
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
     * @param name The language configuration file name.
     * @param error The message context.
     * @return A unique instance for each language or NULL if an error occured.
     */
    static const Plexer* Create(std::string name, ErrorList& error);

private:
    Plexer();                                   ///< Constructor (not defined or used).
    Plexer(Plexer&);                            ///< Copy constructor (not defined or used).
    /** The constructor.
     * @param name The file to scan.
     * @param errors The error message context.
     */
    Plexer(std::string name, ErrorList& errors);
    /** Destructor.
     */
    ~Plexer() { delete options.reservedWords; delete options.tokens; }

public:
    ErrorList& errors;                          ///< The error list.
    Options options;                            ///< Preprocessor options.
    array<Macro> macros;                        ///< The predefined macro table.
    array<std::string> includes;                ///< The include path table.
    /** Configuration file token identifiers.
     */
    enum CFGTokens {
        STRING = PPStream::CTNEXTOKEN,          ///< A character string.
        CHARACTER,                              ///< A character literal.
        INTEGER,                                ///< An integer constant.
        FLOAT,                                  ///< A floating point constant.
        IDENTIFIER,                             ///< An identifier.
        LBRACE,                                 ///< '{'
        RBRACE,                                 ///< '}'
        COMMA,                                  ///< ','
        ASSIGN,                                 ///< '='
        SEMICOLON,                              ///< ';'
        RANGE,                                  ///< '..'
    };
    /** A parsing function.
     * @param pp The preprocessor.
     * @param env The lexer environment.
     * @param errors The message context.
     * @param data Parsing specific context.
     */
    typedef void Parser(PP& pp, Plexer& env, ErrorList& errors, void* data);
    /** Define a language sentence parser.
     * @param name The token starting the sentence.
     * @param handler The function to call to parse the sentence.
     */
    void first(std::string name, Parser* handler);
    /** Parse a language file.
     * @param name The name of the file.
     * @param data Parsing specific context.
     * @param macros Where to put macro definitions.
     * @return true if the parse succeeded.
     */
    bool parse(std::string name, void* data, array<Macro>* macros = NULL);

private:
    std::string name;                           ///< The language name.
    static array<Plexer*> languages;            ///< The known language array.
    void CFGsetupOptions();                     ///< Set preprocessor options.
    static Options CFGoption;                   ///< Default language file options.
    static const WordAssoc CFGreservedWords[];  ///< Configuration file reserved words.
    static const WordAssoc CFGtokens[];         ///< Configuration file tokens.
    /** Get configuration file token names.
     * @param value The token value.
     * @param context The parsing context.
     * @return The string representing the token.
     */
    static const char* CFGtokenName(int value, void* context);
    static Bracket CFGcomments[];               ///< Configuration file comments.

    /* Extensible parser handling.
     */
    /** A Parser entry.
     */
    class Entry {
    public:
        /** Construct an entry.
         * @param string The start token of a sentence.
         * @param parser The parsing function.
         */
        Entry(std::string string, Parser* parser)
            : parser(parser), string(string) { }
        /** Get the token that starts the sentence.
         * @return The token string.
         */
        std::string& name() { return string; }
        Parser* parser;                         ///< The parsing function.
    private:
        Entry();                                ///< The default constuctor (not defined or used).
        std::string string;                     ///< The starting token.
    };

    Table<Entry*> parsers;                      ///< The parser table.

    /** A token definition.
     */
    struct TokenDefinition : Token {
        /** The definition constructor.
         * @param value The token to define.
         */
        TokenDefinition(const Token& value)
            : Token(value), value(0), keyword(false)
            { }
        /** Get the name of this token.
         * @return The token name.
         */
        std::string& name()
            { return string; }
        array<Token*> regex;                    ///< Defining regular expression(s), if any.
        Matcher::Input value;                   ///< The token value.
        bool keyword;                           ///< true if the token is a keyword.
    };

    /** The configuration file parsing state.
     */
    struct State {
        /** The constructor.
         */
        State()
            : tokenCount(PPStream::CTNEXTOKEN), keywordCount(0), nextToken(PPStream::CTNEXTOKEN)
            { }
        Table<TokenDefinition*> tokenTable;     ///< Token hash table.
        int tokenCount;                         ///< Number of tokens.
        int keywordCount;                       ///< Number of keywords.
        Matcher::Input nextToken;               ///< Next token number.
        Plexer* language;                       ///< The language to build.
    };
    /** Define a token.
     * @param info The token.
     * @param state The parsing state.
     * @param keyword true if the token is a keyword.
     * @return A pointer to the definition.
     */
    static TokenDefinition* newToken(Token& info, State* state, bool keyword);
    /* Set up token information.
     * @param sp The parsing state.
     */
    void setupTokens(State* sp);
    /* Configuration language sentence handlers.
     */
    static Parser parseTokens;                  ///< Parse tokens and keywords.
    static Parser parseComments;                ///< Parse comments.
    static Parser parseIncludes;                ///< Parse include directories.
    static Parser parseNeedwhitespace;          ///< Set the needwhitespace flag.
    WordAssoc* reservedWords;                   ///< Reserved words defined in this language.
    WordAssoc* tokens;                          ///< Tokens defined in this language.
};

};

#endif
