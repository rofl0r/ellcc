#include "pwPP.h"

namespace pw {

/** A Configuration file handler.
 */
class Config {
public:
    /** Create configuration file parser definition.
     * @param name The configuration name.
     * @return A unique instance for each configuration or NULL if an error occured.
     */
    static const Config* Create(std::string name, ErrorList& error);

private:
    /** Constructor (not defined or used).
     */
    Config();
    /** Copy constructor (not defined or used).
     */
    Config(Config&);
    /** Constructor.
     */
    Config(std::string name, ErrorList& errors);
    /** Destructor.
     */
    virtual ~Config() { }

public:
    /** The error list.
     */
    ErrorList& errors;

    /** Preprocessor options.
     */
    Options options;

    /** Configuration file token identifiers.
     */
    enum Tokens {
        STRING = PPStream::CTNEXTOKEN, CHARACTER, INTEGER, FLOAT, IDENTIFIER,
        LBRACE, RBRACE, COMMA, ASSIGN, RANGE, 
    };
    /** A keyword parsing function.
     */
    typedef void Parser(PP& pp, Config& env, ErrorList& errors, void* data);
    /** Define a configuration keyword.
     */
    void keyword(std::string name, Parser* handler);
    /** Parse a configuration file.
     */
    bool parse(std::string name, void* data);

private:
    /** The language name.
     */
    std::string name;
    /** The known language array.
     */
    static array<Config*> languages;

    /** Set preprocessor options.
     */
    void setupOptions();

    /** Default configuration file options.
     */
    static Options configOptions;

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
    static const Bracket CFGcomments[];

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
        Config* config;                             	// The configuration to build.
    };

    static TokenDefinition* newToken(Token& info, State* state, bool keyword);

    /* Set up token information.
     */
    void setupTokens(State* sp);
    
    /* Configuration keyword handlers.
     */
    /** Parse tokens and keywords.
     */
    static Parser parseTokens;
    /** Set the needwhitespace flag.
     */
    static Parser parseNeedwhitespace;
    WordAssoc* reservedWords;
    WordAssoc* tokens;
};

/* The known language array.
 */
array<Config*> Config::languages;

/* Construct a language entry.
 */
Config::Config(std::string name, ErrorList& errors)
    : errors(errors), name(name), reservedWords(NULL), tokens(NULL)
{
}

/* convert - Convert a string to the "real" value.
 */
static std::string convert(std::string& string)
{
    std::string result;
    int next;
    bool l = false;              // An L string.
    int quote;
    int i;
    int value;
    const char *p = string.c_str();
    bool haveendquote = false;

    next = *p++;
    if (next == '\0') {
        return result;          // End of string.
    }

    if (next == 'L') {
        l = true;
        next = *p++;
    }

    if (next != '"' && next != '\'') {
        // No quote present, copy the string verbatim.

        result = p;
        return result;
    }

    quote = next;       // Remember the quote character.
    for (;;) {
        next = *p++;
        if (!next) {
            // End of the string.
            break;
        }
        if (next == quote) {
            // Have a quote character
            if (!haveendquote) {
                // This is an end quote.
                haveendquote = true;
            } else {
                // This is a new start quote.
                haveendquote = false;
            }
        } else if (!haveendquote) {
            // Characters between quotes.
            if (next == '\\') {
                // This is an escape sequence.
                next = *p++;
                switch (next) {
                case '\0':
                    return result;      // This is a malformed constant.
                case '\'':
                    result += '\'';
                    break;
                case '"':
                    result += '"';
                    break;
                case '?':
                    result += '?';
                    break;
                case '\\':
                    result += '\\';
                    break;
                case 'a':
                    result += '\a';
                    break;
                case 'b':
                    result += '\b';
                    break;
                case 'f':
                    result += '\f';
                    break;
                case 'n':
                    result += '\n';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 'v':
                    result += '\v';
                    break;
                case 'x':
                    // Hexadecimal constant.
                    value = 0;
                    i = 0;              // Character counter.
                    next = *p++;
                    while (isxdigit(next)) {
                        value <<= 4;
                        if (isdigit(next)) {
                            next -= '0';
                        } else {
                            next = toupper(next) - 'A' + 10;
                        }
                        value += next;
                        ++i;
                        if (i == 2) {
                            // Send the hex character.
                            result += value;
                            i = 0;
                        }
                        next = *p++;
                    }
                    break;
                case '0': case '1': case '2': case '3':
                case '4': case '5': case '6': case '7':
                    // Octal constant.
                    value = 0;
                    i = 3;              // Max digit counter.
                    while (i && next >= '0' && next <= '7') {
                        value <<= 3;
                        next -= '0';
                        value += next;
                        --i;
                        next = *p++;
                    }
                    result += value;
                    break;
                default:
                    result += next;
                    break;
                }
            } else {
                // Send the character.
                result += next;
            }
        }
    }

    return result;
}

/* Check and setup the tokens for use.
 */
void Config::setupTokens(State* sp)
{
    TokenDefinition* tp;
    std::string temp;
    Token regex;
    Matcher *rm, *tm;
    int tindex, kindex;

#if RICH
    if (lp->name.length() == 0) {
        cp->error(pwError::ERROR,
                  cp->info.startline, cp->info.startcolumn,
                  cp->info.endline, cp->info.endcolumn,
                  "No name defined for language.");
    }

    if (lp->description.length() == 0) {
        cp->error(pwError::WARNING,
                  cp->info.startline, cp->info.startcolumn,
                  cp->info.endline, cp->info.endcolumn,
                  "No description defined for language.");
    }
#endif

    // Allocate the token and keyword translation tables.
    tindex = kindex = 0;
    if (sp->tokenCount) {
        static WordAssoc pptokens[] = {
            pwPSCANTOKENS
            { NULL, 0}
        };
        WordAssoc *wp;

        // Define the preprocessor intrinsic tokens.
        sp->config->tokens = new WordAssoc[sp->tokenCount + 1];
        for (wp = pptokens; wp->word; ++wp) {
            sp->config->tokens[tindex].word = strdup(wp->word);
            sp->config->tokens[tindex].token = wp->token;
            ++tindex;
        }
    }

    if (sp->keywordCount) {
        sp->config->reservedWords = new WordAssoc[sp->keywordCount + 1];
    }

    rm = NULL;
    tm = NULL;
    // Build the token and keyword state machine.
    for (int i = 0; i < sp->tokenTable.size(); ++i) {
        tp = sp->tokenTable[i];
        if (tp->string[0] == '\'' || tp->string[0] == '"') {
            temp = convert(tp->string);
        } else {
            temp = tp->string;
        }

        if (tp->keyword) {
            // This is keyword.
            if (rm == NULL) {
                rm = sp->config->options.reservedWords = new Matcher("reserved words", Matcher::CHARSIZE,
                                                             NULL, NULL, 0);
            }

            // Add to the reserved word table.
            rm->addWord(temp, tp->value);
            sp->config->reservedWords[kindex].word = strdup(tp->string.c_str());
            sp->config->reservedWords[kindex].token = tp->value;
            ++kindex;
        } else {
            // This is a token.
            if (tm == NULL) {
                tm = sp->config->options.tokens = new Matcher("tokens", Matcher::CHARSIZE,
                                                       NULL, NULL, 0);
            }

            // Add to the token table.
            if (tp->regex.size()) {
                int j;

                for (j = 0; j < tp->regex.size(); ++j) {
                    // Add a regular expression.
                    regex = *tp->regex[j];
                    regex.string = convert(regex.string);	// Translate escape sequences.
                    MatchNode regexp(regex.string);
                    tm->addTree(&regexp, tp->value);
                }
            } else {
                // A simple token.
                tm->addWord(temp, tp->value);
            }

            sp->config->tokens[tindex].word = strdup(tp->string.c_str());
            sp->config->tokens[tindex].token = tp->value;
            ++tindex;
        }

        // p points to the token string, tp->value is its token value.

        // Assign values to tokens needed by the preprocessor.
        if (sp->config->options.INTEGER == PPStream::NONE && temp == "INTEGER") {
            sp->config->options.INTEGER = tp->value;
        }
        if (sp->config->options.CHARACTER == PPStream::NONE && temp == "CHARACTER") {
            sp->config->options.CHARACTER = tp->value;
        }
        if (sp->config->options.FLOAT == PPStream::NONE && temp == "FLOAT") {
            sp->config->options.FLOAT = tp->value;
        }
        if (sp->config->options.STRING == PPStream::NONE && temp == "STRING") {
            sp->config->options.STRING = tp->value;
        }
        if (sp->config->options.IDENTIFIER == PPStream::NONE && temp == "IDENTIFIER") {
            sp->config->options.IDENTIFIER = tp->value;
        }
    }

    // Terminate the token and keyword lookup tables.
    if (tindex) {
        sp->config->tokens[tindex].word = NULL;
        sp->config->tokens[tindex].token = 0;
    }

    if (kindex) {
        sp->config->reservedWords[kindex].word = NULL;
        sp->config->reservedWords[kindex].token = 0;
    }

    if (sp->config->options.IDENTIFIER == PPStream::NONE && sp->keywordCount) {
        errors.add(Error::ERROR,
                   0, 0, 0, 0,
                   "Language contains keywords but no IDENTIFIER token is defined.");
    }
}

/* Create a language entry.
 */
const Config* Config::Create(std::string name, ErrorList& errors)
{
    if (languages.size() == 0) {
        // Create the configuration file scanner.
        languages[0] = new Config(std::string("config"), errors);
        languages[0]->setupOptions();
        languages[0]->keyword("needwhitespace", parseNeedwhitespace);
        languages[0]->keyword("tokens", parseTokens);
    }

    int i;
    for (i = 0; i < languages.size(); ++i) {
        if (languages[i]->name == name) {
            return languages[i];
        }
    }

    // Parse a config file.
    State* sp = new State();
    Config* lp = new Config(name, errors);
    sp->config = lp;
    errors.recentErrors = false;
    if (languages[0]->parse(name, sp)) {
        languages[0]->setupTokens(sp);
    }
    if (!errors.recentErrors) {
        languages[i] = lp;
    } else {
        // Some error occured.
        delete lp;
        lp = NULL;
    }

    delete sp;
    return lp;
}

const WordAssoc Config::CFGreservedWords[] = {
    { NULL,  0 },
};

#define PW_LEX_TOKENS \
    { "{",   LBRACE },          \
    { "}",   RBRACE },          \
    { ",",   COMMA },           \
    { "=",   ASSIGN },          \
    { "..",  RANGE}

const WordAssoc Config::CFGtokens[] = {
    PW_LEX_TOKENS,
    { " [a-zA-Z_][a-zA-Z_0-9]*", IDENTIFIER },
    { " [1-9][0-9]*([uU]|[lL])*", INTEGER },            	// Decimal integer
    { " 0[xX][0-9a-fA-F]+([uU]|[lL])*", INTEGER },      	// Hexadecimal integer
    { " 0[0-7]*([uU]|[lL])*", INTEGER },                        // Octal integer
    { " [0-9]+\\.[0-9]*([eE][-+]?[0-9]+)?", FLOAT },    	// <digits>.
    { " [0-9]*\\.[0-9]+([eE][-+]?[0-9]+)?",     FLOAT },    	// .<digits>
    { " [0-9]+[eE][-+]?[0-9]+", FLOAT },                        // <digits>e
    { " L?\"(\\\\.|[^\"\n])*\"", STRING },
    { " L?'(\\\\.|[^'\n])*'", CHARACTER },
    { NULL,  0 },
};

const Bracket Config::CFGcomments[] =
{
    { "//", "\n", PPStream::COMMENT },        		// Single line comment.
    { "/*", "*/", PPStream::COMMENT },        		// Multi line comment.
    { NULL, 0,     0 }

};

//
// tokenName - display a token name.
//
const char* Config::CFGtokenName(int value, void* context)
{
    const WordAssoc* wp;
    static const WordAssoc tokens[] = {
        { "<EOF>", PPStream::ENDOFFILE },
        { "NL", PPStream::NL },
        { "WS", PPStream::WS },
        { "COMMENT", PPStream::COMMENT },
        { "STRING", STRING },
        { "CHARACTER", CHARACTER },
        { "INTEGER", INTEGER },
        { "FLOAT", FLOAT },
        { "IDENTIFIER", IDENTIFIER },
        PW_LEX_TOKENS,
        { NULL,  0 }
    };

    for (wp = tokens; wp->word; ++wp) {
        if (wp->token == value) {
            return wp->word;
        }
    }

    // Use the default name.
    return stateValueName(value, context);
}

Options Config::configOptions(
    true,                               			// Trigraphs allowed.
    INTEGER,                            			// Integer token.
    CHARACTER,                          			// Character token.
    FLOAT,                              			// Float token.
    STRING,                             			// String token.
    IDENTIFIER,                         			// Identifier token.
    NULL,                      					// Reserved words in this language.
    NULL,                             				// Tokens in the language.
    CFGcomments);                           			// Comments in the language.

void Config::setupOptions()
{
    pw::Matcher *machine;
    const pw::WordAssoc *wp;

    options = configOptions;				// Set default options.
    if (options.reservedWords == NULL) {
        machine = options.reservedWords = new pw::Matcher("reserved words", pw::Matcher::CHARSIZE,
                                                                NULL,
                                                                NULL, 0);
        for (wp = CFGreservedWords; wp->word; ++wp) {
            machine->addWord(wp->word, wp->token);
        }
    }

    if (options.tokens == NULL) {
        machine = options.tokens = new pw::Matcher("tokens", pw::Matcher::CHARSIZE,
                                                        NULL,
                                                        NULL, 0);
    for (wp = CFGtokens; wp->word; ++wp) {
        if (*wp->word == ' ') {
                // Match a regular expression.
                std::string regstr(wp->word + 1);
                pw::MatchNode regexp(regstr);
                machine->addTree(&regexp, wp->token);
            } else {
                // Match a word.
                machine->addWord(wp->word, wp->token);
            }
        }
    }
    // RICH machine->print(stdout, NULL);
}

/** Define a configuration keyword.
 */
void Config::keyword(std::string name, Parser* handler)
{
    Entry* ep = new Entry(name, handler);
    parsers += ep;
}

/* Parse a configuration file.
 */
bool Config::parse(std::string name, void* data)
{
    pw::PP* pp = new PP(name, errors);
    FILE* fp = NULL;
    if (pp == NULL) {
        errors.add(Error::INTERNAL, __FILE__, __LINE__, 0, 0, 0, "Can't create preprocessor.");
        return false;
    }
    if (!pp->setInput(fp)) {
        errors.add(Error::ERROR, 0, 0, 0, 0, "Can't open %s.", name.c_str());
        delete pp;
        return false;
    }
    
    pp->setOptions(&options);    		// Set pre-processor options.

    pp->getToken();
    errors.file = pp->info.file;
    std::string lastfile;
    for (;;) {
        if (pp->info.token == PPStream::ENDOFFILE) {
            // End of file.
            break;
        }
        lastfile = errors.file;

        // Have a token, check for parse functions.
        Entry* ep = parsers.lookup(pp->info.string);
        if (ep) {
            // A parsing function has been found.
            pp->getToken();
            ep->parser(*pp, *this, errors, data);
        } else {
            // RICH: Define a value.
            pp->getToken();
        }
        errors.file = pp->info.file;       	// Remember the last file for error reporting.
    }

    return true;
}

/** Display a token expected error message.
 */
Error* expectedToken(PP& pp, const char* string)
{
    Error* ep;
    const char* format = "%s expected before \"%s\".";

    if (pp.info.token == PPStream::ENDOFFILE) {
        format = "%s expected before the end of the file.";
    }

    if (!isupper(*string) && *string != '"' && *string != '\'') {
        // Put quotes around the expected token.
        format = "\"%s\" expected before \"%s\".";
        if (pp.info.token == PPStream::ENDOFFILE) {
            format = "\"%s\" expected before the end of the file.";
        }
    }

    ep = pp.error(Error::ERROR,
                  pp.info.startline, pp.info.startcolumn, pp.info.endline, pp.info.endcolumn,
                  format, string, pp.info.string.c_str());
    return ep;
}

/* Complain about an extra token.
 */
Error* extraToken(PP& pp, const char* string)
{
    Error *ep;
    const char *format = "Extra %s found after %s.";

    ep = pp.error(Error::ERROR,
                  pp.info.startline, pp.info.startcolumn, pp.info.endline, pp.info.endcolumn,
                  format, pp.info.string.c_str(), string);
    return ep;
}

/* Create a new token entry.
 */
Config::TokenDefinition* Config::newToken(Token& info, State* state, bool keyword)
{
    TokenDefinition* tp = new TokenDefinition(info);

    tp->value = state->nextToken++;
    tp->keyword = keyword;
    if (keyword) {
        ++state->keywordCount;
    } else {
        ++state->tokenCount;
    }
    state->tokenTable += tp;
    return tp;
}

/* Define tokens and keywords.
 */
void Config::parseTokens(PP& pp, Config& env, ErrorList& errors, void* data)
{
    bool keyword = false;
    State* sp = (State*)data;
    TokenDefinition* tp;
    bool needregex;

    if (pp.info.token != LBRACE) {
        expectedToken(pp, "{");
    } else {
        pp.getToken();
    }

    while (pp.info.token != RBRACE) {
        // Gather all comma separated tokens. We do an identifier check for keywords later.

        if (pp.info.token == COMMA) {
            // This comma is out of place.
            pp.error(Error::ERROR,
                     pp.info.startline, pp.info.startcolumn, pp.info.endline, pp.info.endcolumn,
                     "Unexpected \"%s\".", pp.info.string.c_str());
            pp.getToken();
            continue;
        }

        tp = sp->tokenTable.lookup(pp.info.string);
        if (tp && (keyword || !tp->regex.size())) {
            Error *erp;
            std::string buffer;

            // Token already defined without a regular expression.
            erp = pp.error(Error::ERROR,
                           pp.info.startline, pp.info.startcolumn, pp.info.endline, pp.info.endcolumn,
                           "%s %s has been reused.", keyword ? "Keyword" : "Token", pp.info.string.c_str());
            pp.errorPosition(buffer, tp->file,
                             tp->startline, tp->startcolumn, tp->endline, tp->endcolumn,
                             false);
            Error::info(erp, Error::MORE, "Last use as %s at %s",
                        tp->keyword ? "keyword" : "token", buffer.c_str());
            pp.getToken();
            if (!keyword) {
                // Tokens can be defined as a regular expression.
                if (pp.info.token == ASSIGN) {
                    // Find a regular expression string.
                    pp.getToken();
                    if (pp.info.token == STRING) {
                        pp.getToken();
                    }
                }
            }
            continue;
        }

        if (!tp) {
            // Need a new token definition.
            tp = newToken(pp.info, sp, keyword);   // Define the token.
            needregex = false;
        } else {
            // This token needs a regular expression.
            needregex = true;
        }

        pp.getToken();
        if (!keyword) {
            // Tokens can be defined as a regular expression.
            if (pp.info.token == ASSIGN) {
                // Find a regular expression string.
                needregex = false;
                pp.getToken();
                if (pp.info.token != STRING) {
                    expectedToken(pp, "Regular expression");
                } else {
                    // Add a regular expression to this token's list.
                    tp->regex[tp->regex.size()] = new Token(pp.info);
                    pp.getToken();
                }
            }
        }

        if (needregex) {
            Error *erp;
            std::string buffer;

            // Token already defined with a regular expression.
            erp = pp.error(Error::ERROR,
                           pp.info.startline, pp.info.startcolumn, pp.info.endline, pp.info.endcolumn,
                           "Token %s has been previously defined as a regular expression.", tp->string.c_str());
            pp.errorPosition(buffer, tp->file,
                             tp->startline, tp->startcolumn, tp->endline, tp->endcolumn,
                             false);
            Error::info(erp, Error::MORE, "First defined at %s", buffer.c_str());
            Error::info(erp, Error::MORE, "This definition must also have a regular expression.");
        }

        // Commas separate.
        if (pp.info.token == COMMA) {
            pp.getToken();
        } else {
            if (pp.info.token == RBRACE) {
                break;
            }

            expectedToken(pp, ",");
        }
    }

    if (pp.info.token != RBRACE) {
        expectedToken(pp, "}");
    } else {
        pp.getToken();
    }
}

void Config::parseNeedwhitespace(PP& pp, Config& env, ErrorList& errors, void* data)
{
    State* sp = (State*)data;
}

};

static pw::ErrorList errors;

//
int main(int argc, char** argv)
{
    std::string file(argv[1]);
    pw::PP* pp = new pw::PP(file, errors);
    FILE* fp = NULL;
    if (pp == NULL) exit(1);
    if (!pp->setInput(fp)) {
        fprintf(stderr, "can't open %s\n", argv[1]);
        delete pp;
        exit(1);
    }
    
#if 0
    // Define any predefined macros.
    for (int index = 0; index < lp->macros.size(); ++index) {
        pp->addDefine(lp->macros[index].name, lp->macros[index].definition);
    }
#endif

    const pw::Config* config = pw::Config::Create("c99.cfg", errors);
    if (config) {
        pw::Options options = config->options;
        pp->setOptions(&options);    		// Set pre-processor options.

        pp->getToken(pw::PP::GETALL);
        errors.file = pp->info.file;
        std::string lastfile;
        for (;;) {
            if (pp->info.token == pw::PPStream::ENDOFFILE) {
                // End of file.
                break;
            }
            if (errors.file != lastfile) {
                // Output #line directive if pre-processing.
                lastfile = errors.file;
                fprintf(stdout, "#line %d \"%s\"\n", pp->info.startline, errors.file.c_str());
            }
            fprintf(stdout, "%s", pp->info.string.c_str());
            pp->getToken(pw::PP::GETALL);
            errors.file = pp->info.file;     	// Remember the last file for error reporting.
        }
    }

    int totalerrors = 0;
    for (int j = 0; j < pw::Error::ERRORCNT; ++j) {
        // Calculate the total number of errors.
        totalerrors += errors.errorCount(j);
    }

    if (totalerrors) {
        for (int i = 0; i < pw::Error::ERRORCNT; ++i) {
            const char *name;
            const char *plural;
            int count = errors.errorCount(i);

            if (count == 0)
                continue;

            if (count == 1)
                plural = "";
            else
                plural = "s";

            name = pw::Error::modifier((pw::Error::Type)i);
            fprintf(stdout, "%d %s message%s reported\n", count, name, plural);
        }

        // Show errors.
        errors.sort();
        errors.print(stdout);
    }
}
