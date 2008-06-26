/** A lexer with primative parsing hooks.
 */

#include "pwPlexer.h"

namespace pw {

/* The known language array.
 */
array<Plexer*> Plexer::languages;

/* Construct a language entry.
 */
Plexer::Plexer(std::string name, ErrorList& errors)
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
void Plexer::setupTokens(State* sp)
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
        sp->language->tokens = new WordAssoc[sp->tokenCount + 1];
        for (wp = pptokens; wp->word; ++wp) {
            sp->language->tokens[tindex].word = strdup(wp->word);
            sp->language->tokens[tindex].token = wp->token;
            ++tindex;
        }
    }

    if (sp->keywordCount) {
        sp->language->reservedWords = new WordAssoc[sp->keywordCount + 1];
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
                rm = sp->language->options.reservedWords = new Matcher("reserved words", Matcher::CHARSIZE,
                                                             NULL, NULL, 0);
            }

            // Add to the reserved word table.
            rm->addWord(temp, tp->value);
            sp->language->reservedWords[kindex].word = strdup(tp->string.c_str());
            sp->language->reservedWords[kindex].token = tp->value;
            ++kindex;
        } else {
            // This is a token.
            if (tm == NULL) {
                tm = sp->language->options.tokens = new Matcher("tokens", Matcher::CHARSIZE,
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

            sp->language->tokens[tindex].word = strdup(tp->string.c_str());
            sp->language->tokens[tindex].token = tp->value;
            ++tindex;
        }

        // Assign values to tokens needed by the preprocessor.
        if (sp->language->options.INTEGER == PPStream::NONE && temp == "INTEGER") {
            sp->language->options.INTEGER = tp->value;
        }
        if (sp->language->options.CHARACTER == PPStream::NONE && temp == "CHARACTER") {
            sp->language->options.CHARACTER = tp->value;
        }
        if (sp->language->options.FLOAT == PPStream::NONE && temp == "FLOAT") {
            sp->language->options.FLOAT = tp->value;
        }
        if (sp->language->options.STRING == PPStream::NONE && temp == "STRING") {
            sp->language->options.STRING = tp->value;
        }
        if (sp->language->options.IDENTIFIER == PPStream::NONE && temp == "IDENTIFIER") {
            sp->language->options.IDENTIFIER = tp->value;
        }
    }

    // Terminate the token and keyword lookup tables.
    if (tindex) {
        sp->language->tokens[tindex].word = NULL;
        sp->language->tokens[tindex].token = 0;
    }

    if (kindex) {
        sp->language->reservedWords[kindex].word = NULL;
        sp->language->reservedWords[kindex].token = 0;
    }

    if (sp->language->options.IDENTIFIER == PPStream::NONE && sp->keywordCount) {
        errors.add(Error::ERROR,
                   0, 0, 0, 0,
                   "Language contains keywords but no IDENTIFIER token is defined.");
    }
}

/* Create a language entry.
 */
const Plexer* Plexer::Create(std::string name, ErrorList& errors)
{
    if (languages.size() == 0) {
        // Create the language file scanner.
        languages[0] = new Plexer(std::string("cfg"), errors);
        languages[0]->CFGsetupOptions();
        languages[0]->first("tokens", parseTokens);
        languages[0]->first("comment", parseComments);
        languages[0]->first("includes", parseIncludes);
        languages[0]->first("needwhitespace", parseNeedwhitespace);
    }

    int i;
    for (i = 0; i < languages.size(); ++i) {
        if (languages[i]->name == name) {
            return languages[i];
        }
    }

    // Parse a language file.
    State* sp = new State();
    Plexer* lp = new Plexer(name, errors);
    sp->language = lp;
    errors.recentErrors = false;
    if (languages[0]->parse(name, sp, &lp->macros)) {
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

const WordAssoc Plexer::CFGreservedWords[] = {
    { NULL,  0 },
};

#define PW_LEX_TOKENS \
    { "{",   LBRACE },          \
    { "}",   RBRACE },          \
    { ",",   COMMA },           \
    { "=",   ASSIGN },          \
    { ";",   SEMICOLON },       \
    { "..",  RANGE }

const WordAssoc Plexer::CFGtokens[] = {
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

Bracket Plexer::CFGcomments[] =
{
    { "//", "\n", PPStream::COMMENT },        		// Single line comment.
    { "/*", "*/", PPStream::COMMENT },        		// Multi line comment.
    { NULL, 0,     0 }

};

//
// tokenName - display a token name.
//
const char* Plexer::CFGtokenName(int value, void* context)
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

Options Plexer::CFGoption(
    true,                               			// Trigraphs allowed.
    INTEGER,                            			// Integer token.
    CHARACTER,                          			// Character token.
    FLOAT,                              			// Float token.
    STRING,                             			// String token.
    IDENTIFIER,                         			// Identifier token.
    NULL,                      					// Reserved word matcher for this language.
    NULL,                             				// Tokens  matcher for the language.
    CFGcomments);                           			// Comments in the language.

void Plexer::CFGsetupOptions()
{
    pw::Matcher *machine;
    const pw::WordAssoc *wp;

    options = CFGoption;				// Set default options.
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

/** Define a language sentence parser.
 */
void Plexer::first(std::string name, Parser* handler)
{
    Entry* ep = new Entry(name, handler);
    parsers += ep;
}

/** Display a token expected error message.
 */
static Error* expectedToken(PP& pp, const char* string)
{
    Error* ep;
    const char* format;

    if (pp.info.token == Plexer::STRING || pp.info.token == Plexer::CHARACTER) {
        format = "%s expected before %s.";
    } else {
        format = "%s expected before \"%s\".";
    }

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

/* Complain about an unexpected token.
 */
static Error* unexpectedToken(PP& pp, const char* string = "")
{
    Error *ep;
    const char *format;

    if (pp.info.token == Plexer::STRING || pp.info.token == Plexer::CHARACTER) {
        format = "Unexpected token %s found%s.";
    } else {
        format = "Unexpected token \"%s\" found%s.";
    }

    ep = pp.error(Error::ERROR,
                  pp.info.startline, pp.info.startcolumn, pp.info.endline, pp.info.endcolumn,
                  format, pp.info.string.c_str(), string);
    return ep;
}

/* Parse a language file.
 */
bool Plexer::parse(std::string name, void* data, array<Macro>* macros)
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
            // We have an unexpected token.
            unexpectedToken(*pp);
            pp->getToken();

            // Scan until a semicolon or known start token is found.
            while (   pp->info.token != SEMICOLON
                   && pp->info.token != PPStream::ENDOFFILE
                   && parsers.lookup(pp->info.string) == NULL) {
                pp->getToken();
            }

            if (pp->info.token == SEMICOLON) {
                pp->getToken();
            }
            continue;
        }
        if (pp->info.token != SEMICOLON) {
            expectedToken(*pp, ";");
        } else {
            pp->getToken();
        }
    }

    if (macros) {
        // Capture macros that have been defined.
        for (int i = 0; i < pp->macros.size(); ++i) {
            if (pp->macros[i]->type != Macro::DEFINED_MACRO) {
                // Grab only user defined macros.
                continue;
            }
            (*macros)[(*macros).size()] = *pp->macros[i];
        }
    }

    delete pp;
    return true;
}

/* Create a new token entry.
 */
Plexer::TokenDefinition* Plexer::newToken(Token& info, State* state, bool keyword)
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
void Plexer::parseTokens(PP& pp, Plexer& env, ErrorList& errors, void* data)
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
            unexpectedToken(pp);
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

/* Define comments.
 */
void Plexer::parseComments(PP& pp, Plexer& env, ErrorList& errors, void* data)
{
    State* sp = (State*)data;
    std::string temp;
    const char *start = NULL, *end;
    int count;
    Bracket *bp;

    if (pp.info.token != LBRACE) {
        expectedToken(pp, "{");
    } else {
        pp.getToken();
    }

    while (pp.info.token != RBRACE) {
        // Gather all comma separated comment definitions.

        if (pp.info.token == COMMA) {
            // This comma is out of place.
            unexpectedToken(pp);
            pp.getToken();
            continue;
        }

        if (pp.info.token == STRING) {
            temp = convert(pp.info.string);
            start = strdup(temp.c_str());
            if (sp->language->options.comments == NULL) {
                // No comments, yet.
                count = 1;
            } else {
                // Count the current number of entries.
                for (count = 1, bp = sp->language->options.comments; bp->start; ++count, ++bp) {
                    if (strcmp(start, bp->start) == 0) {
                        pp.error(Error::ERROR,
                                 pp.info.startline, pp.info.startcolumn, pp.info.endline, pp.info.endcolumn,
                                 "Comment start sequence %s has already been used.", pp.info.string.c_str());
                    }
                    continue;
                }
            }

            pp.getToken();
        } else {
            expectedToken(pp, "String");
            pp.getToken();
            continue;
        }

        if (pp.info.token == RANGE) {
            pp.getToken();
        } else {
            expectedToken(pp, "..");
            continue;
        }

        if (pp.info.token == STRING) {
            temp = convert(pp.info.string);
            end = strdup(temp.c_str());
            pp.getToken();
        } else {
            expectedToken(pp, "String");
            pp.getToken();
            continue;
        }

        // Have a start and end marker. Add to the comment list.
        sp->language->options.comments = (Bracket *)realloc(sp->language->options.comments,
                                                           (count + 1) * sizeof(Bracket));
        --count;
        sp->language->options.comments[count].start = strdup(start);
        sp->language->options.comments[count].end = strdup(end);
        sp->language->options.comments[count].token = PPStream::COMMENT;
        ++count;
        sp->language->options.comments[count].start = NULL;
        sp->language->options.comments[count].end = NULL;
        sp->language->options.comments[count].token = 0;

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

/* Define include directories.
 */
void Plexer::parseIncludes(PP& pp, Plexer& env, ErrorList& errors, void* data)
{
    State* sp = (State*)data;
    std::string name;

    if (pp.info.token != LBRACE) {
        expectedToken(pp, "{");
    } else {
        pp.getToken();
    }

    while (pp.info.token != RBRACE) {
        // Gather all comma separated comment definitions.

        if (pp.info.token == COMMA) {
            // This comma is out of place.
            unexpectedToken(pp);
            pp.getToken();
            continue;
        }

        if (pp.info.token == STRING) {
            name = convert(pp.info.string);
            pp.getToken();
        } else {
            expectedToken(pp, "String");
            pp.getToken();
            continue;
        }

        // Have a name. Add to the include list.
        int size = sp->language->includes.size();
        sp->language->includes[size] = name;

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

void Plexer::parseNeedwhitespace(PP& pp, Plexer& env, ErrorList& errors, void* data)
{
    State* sp = (State*)data;
}

};
