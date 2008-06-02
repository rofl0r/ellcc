#include "pwPP.h"

static bool haveErrors;
static pw::ErrorList *errors;                        // Errors encountered.
static int errorcount[pw::Error::ERRORCNT];          // Number of errors encountered.

namespace pw {

/** A Language definition.
 */
class PPLanguage {
public:
private:
    /** Reserved words in the language.
     */
    virtual const WordAssoc* getReservedWords() { return NULL; }
    /** Tokens in the language.
     */
    virtual const WordAssoc* getTokensWords() { return NULL; }
    /** Comments in the language.
     */
    virtual const Bracket* getComments() { return NULL; }
};

};

static pw::MacroTable macros;
static pw::TokenInfo info;                           // Information about the token.
static std::string pplastfile;
static std::string lastfile;

enum tokens {
    STRING = pw::PPStream::CTNEXTOKEN, CHARACTER, INTEGER, FLOAT, IDENTIFIER,
    LBRACE, RBRACE, COMMA, ASSIGN, RANGE, 
};

static const pw::WordAssoc reservedWords[] = {
    { NULL,  0 },
};

#define PW_LEX_TOKENS \
    { "{",   LBRACE },          \
    { "}",   RBRACE },          \
    { ",",   COMMA },           \
    { "=",   ASSIGN },          \
    { "..",  RANGE}

static const pw::WordAssoc tokens[] = {
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

static const pw::Bracket comments[] =
{
    { "//", "\n", pw::PPStream::COMMENT },        		// Single line comment.
    { "/*", "*/", pw::PPStream::COMMENT },        		// Multi line comment.
    { NULL, 0,     0 }
};

//
// tokenName - display a token name.
//
static const char* tokenName(int value, void* context)
{
    const pw::WordAssoc* wp;
    static const pw::WordAssoc tokens[] = {
        { "<EOF>", pw::PPStream::ENDOFFILE },
        { "NL", pw::PPStream::NL },
        { "WS", pw::PPStream::WS },
        { "COMMENT", pw::PPStream::COMMENT },
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
    return pw::stateValueName(value, context);
}

static pw::Options options = {
    true,                               			// Trigraphs allowed.
    INTEGER,                            			// Integer token.
    CHARACTER,                          			// Character token.
    FLOAT,                              			// Float token.
    STRING,                             			// String token.
    IDENTIFIER,                         			// Identifier token.
    NULL,                      					// Reserved words in this language.
    NULL,                             				// Tokens in the language.
    comments,                           			// Comments in the language.
};

//
// verror - Handle an error.
//
pw::Error* verror(pw::Error::Type type,
                           const std::string& file,
                           int startline, int startcolumn, int endline, int endcolumn,
                           const char *string, va_list ap)
{
    pw::Error *ep = errors->add(type, file, startline, startcolumn, endline, endcolumn, string, ap);
    ++errorcount[type];                     // Increment the error count for this type.
    if (ep && ep->isError())
        haveErrors = true;                      // Inhibit subsequent processing.

    if (type == pw::Error::FATAL)
          exit(1);
//        longjmp(fatal, 1);                      // Abort processing.

    return ep;
}

class PP : public pw::PP
{
public:
    PP(std::string& name) : pw::PP(name, ::macros)
        { }
    pw::Error* error(pw::Error::Type type,
                   int startline, int startcolumn, int endline, int endcolumn,
                   const char* format, ...);
    void errorPosition(std::string& buffer, const std::string& file,
                       int startline, int startcolumn, int endline, int endcolumn, bool trailer);
private:
};

//
// error - Handle an error.
//
pw::Error* PP::error(pw::Error::Type type,
                       int startline, int startcolumn, int endline, int endcolumn,
                       const char *string, ...)
{
    va_list ap;
    pw::Error *ep;

    va_start(ap, string);
    ep = verror(type, lastfile, startline, startcolumn, endline, endcolumn, string, ap);
    va_end(ap);

    return ep;
}

//
// errorPosition - Get an error position.
//
void PP::errorPosition(std::string& buffer, const std::string& file,
                           int startline, int startcolumn, int endline, int endcolumn, bool trailer)
{
    errors->position(buffer, file, startline, startcolumn, endline, endcolumn, trailer);
}

static void setupStateMachines()
{
    pw::Matcher *machine;
    const pw::WordAssoc *wp;

    if (options.reservedwords == NULL) {
        machine = options.reservedwords = new pw::Matcher("reserved words", pw::Matcher::CHARSIZE,
                                                                NULL,
                                                                NULL, 0);
        for (wp = reservedWords; wp->word; ++wp) {
            machine->addWord(wp->word, wp->token);
        }
    }

    if (options.tokens == NULL) {
        machine = options.tokens = new pw::Matcher("tokens", pw::Matcher::CHARSIZE,
                                                        NULL,
                                                        NULL, 0);
    for (wp = tokens; wp->word; ++wp) {
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

int main(int argc, char** argv)
{
    std::string file(argv[1]);
    errors = new pw::ErrorList;
    PP* pp = new PP(file);
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

    setupStateMachines();
    pp->setOptions(&options);    // Set pre-processor options.

    pp->getToken(info, pw::PP::GETALL);
    lastfile = info.file;                       // Remember the last file for error reporting.
    for (;;) {
        if (info.token == pw::PPStream::ENDOFFILE) {
            // End of file.
            break;
        }
        if (lastfile != pplastfile) {
            // Output #line directive if pre-processing.
            pplastfile = lastfile;
            fprintf(stdout, "#line %d \"%s\"\n", info.startline, lastfile.c_str());
        }
        fprintf(stdout, "%s", info.string.c_str());
        pp->getToken(info, pw::PP::GETALL);
        lastfile = info.file;                       // Remember the last file for error reporting.
    }

    int totalerrors = 0;
    for (int j = 0; j < pw::Error::ERRORCNT; ++j) {
        // Calculate the total number of errors.
        totalerrors += errorcount[j];
    }

    if (totalerrors) {
        for (int i = 0; i < pw::Error::ERRORCNT; ++i) {
            const char *name;
            const char *plural;
            int count = errorcount[i];

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
        errors->sort();
        errors->print(stdout);
    }
}
