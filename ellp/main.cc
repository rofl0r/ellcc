#include "Ellp.h"

static EllpMacroTable macros;
static pw::ErrorList *errors;                        // Errors encountered.
static int errorcount[pw::Error::ERRORCNT];          // Number of errors encountered.
static EllpTokenInfo info;                           // Information about the token.
static bool haveErrors;
static std::string pplastfile;
static std::string lastfile;

enum tokens {
    STRING = EllpStream::CTNEXTOKEN, CHARACTER, INTEGER, FLOAT, IDENTIFIER
};

static EllpWordAssoc reservedWords[] = {
    { NULL,  0 },
};

static EllpWordAssoc tokens[] = {
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

static EllpBracket comments[] =
{
    { "//", "\n", EllpStream::COMMENT },        		// Single line comment.
    { "/*", "*/", EllpStream::COMMENT },        		// Multi line comment.
    { NULL, 0,     0 }
};

static EllpOptions options = {
    true,                               			// Trigraphs allowed.
    INTEGER,                            			// Integer token.
    CHARACTER,                          			// Character token.
    FLOAT,                              			// Float token.
    STRING,                             			// String token.
    IDENTIFIER,                         			// Identifier token.
    NULL,                      					// Reserved words in this language.
    NULL,                             				// Tokens in the language.
    NULL,                           				// Comments in the language.
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

class PP : public Ellp
{
public:
    PP(std::string& name) : Ellp(name, ::macros)
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
    const EllpWordAssoc *wp;

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
                std::string regstr;
                regstr = wp->word + 1;
                pw::MatchNode regexp(regstr);
                machine->addTree(&regexp, wp->token);
            } else {
                // Match a word.
                machine->addWord(wp->word, wp->token);
            }
        }
    }
}

int main(int argc, char** argv)
{
    std::string file(argv[1]);
    errors = new pw::ErrorList;
    Ellp* pp = new PP(file);
    FILE* fp = NULL;
    if (pp == NULL) exit(1);
    if (!pp->setInput(fp)) {
        fprintf(stderr, "can't open %s\n", argv[1]);
    }
    
#if 0
    // Define any predefined macros.
    for (int index = 0; index < lp->macros.size(); ++index) {
        pp->addDefine(lp->macros[index].name, lp->macros[index].definition);
    }
#endif

    setupStateMachines();
    pp->setOptions(&options);    // Set pre-processor options.

    pp->getToken(info, Ellp::GETALL);
    lastfile = info.file;                       // Remember the last file for error reporting.
    for (;;) {
        if (info.token == EllpStream::ENDOFFILE) {
            // End of file.
            break;
        }
        if (lastfile != pplastfile) {
            // Output #line directive if pre-processing.
            pplastfile = lastfile;
            fprintf(stdout, "#line %d \"%s\"\n", info.startline, lastfile.c_str());
        }
        fprintf(stdout, "%s", info.string.c_str());
        pp->getToken(info, Ellp::GETALL);
        lastfile = info.file;                       // Remember the last file for error reporting.
    }
}
