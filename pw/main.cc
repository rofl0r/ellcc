#include "pwPP.h"

static bool haveErrors;
static pw::ErrorList *errors;                        // Errors encountered.
static int errorcount[pw::Error::ERRORCNT];          // Number of errors encountered.

namespace pw {

/** A Configuration file handler.
 */
class Config {
public:
    /** Create a configuration file parser.
     * @param name The configuration name.
     * @return A unique instance for each configuration or NULL if an error occured.
     */
    static const Config* Create(char* name, ErrorList& errors);
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
    enum tokens {
        STRING = PPStream::CTNEXTOKEN, CHARACTER, INTEGER, FLOAT, IDENTIFIER,
        LBRACE, RBRACE, COMMA, ASSIGN, RANGE, 
    };
    /** A keyword parsing function.
     */
    typedef void Parser(Config& env, ErrorList& errors, void* data);
    /** Define a configuration keyword.
     */
    void keyword(std::string name, Parser* handler, void* data = 0);
    /** Parse a configuration file.
     */
    Config* parse(std::string name);

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
    static const WordAssoc reservedWords[];
    /** Configuration file tokens.
     */
    static const WordAssoc tokens[];
    /** Configuration file token names.
     */
    static const char* tokenName(int value, void* context);
    /** Configuration file comments.
     */
    static const Bracket comments[];

    /* Configuration keyword handlers.
     */
    /** Set the needwhitespace flag.
     */
    static Parser needwhitespace;
};

/* The known language array.
 */
array<Config*> Config::languages;

/* Construct a language entry.
 */
Config::Config(std::string name, ErrorList& errors) : name(name), errors(errors)
{
}

/* Create a language entry.
 */
const Config* Config::Create(char* name, ErrorList& errors)
{
    return Create(std::string(name), errors);
}

/* Create a language entry.
 */
const Config* Config::Create(std::string name, ErrorList& errors)
{
    if (languages.size() == 0) {
        // Create the configuration file scanner.
        languages[0] = new Config(std::string("config"), errors);
        languages[0]->setupOptions();
        languages[0]->keyword("needwhitespace", needwhitespace);
    }

    int i;
    for (i = 0; i < languages.size(); ++i) {
        if (languages[i]->name == name) {
            return languages[i];
        }
    }

    // Parse a config file.
    Config* lp = languages[0]->parse(name);
    if (lp) {
        languages[i] = lp;
    }
    return lp;
}

const WordAssoc Config::reservedWords[] = {
    { NULL,  0 },
};

#define PW_LEX_TOKENS \
    { "{",   LBRACE },          \
    { "}",   RBRACE },          \
    { ",",   COMMA },           \
    { "=",   ASSIGN },          \
    { "..",  RANGE}

const WordAssoc Config::tokens[] = {
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

const Bracket Config::comments[] =
{
    { "//", "\n", PPStream::COMMENT },        		// Single line comment.
    { "/*", "*/", PPStream::COMMENT },        		// Multi line comment.
    { NULL, 0,     0 }

};

//
// tokenName - display a token name.
//
const char* Config::tokenName(int value, void* context)
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
    comments);                           			// Comments in the language.

void Config::setupOptions()
{
    pw::Matcher *machine;
    const pw::WordAssoc *wp;

    options = configOptions;				// Set default options.
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

/** Define a configuration keyword.
 */
void Config::keyword(std::string name, Parser* handler, void* data)
{
}

/* Parse a configuration file.
 */
Config* Config::parse(std::string name)
{
    Config* lp = new Config(name, errors);
    return lp;
}

void Config::needwhitespace(Config& env, ErrorList& errors, void* data)
{
}

};

static pw::MacroTable macros;
static pw::TokenInfo info;                           // Information about the token.
static std::string pplastfile;
static std::string lastfile;

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

    const pw::Config* config = pw::Config::Create("config", *errors);
    pw::Options options = config->options;
    pp->setOptions(&options);    		// Set pre-processor options.

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
