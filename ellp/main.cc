#include "Ellp.h"

static EllpMacroTable macros;
static EllErrorList *errors;                        // Errors encountered.
static int errorcount[EllError::ERRORCNT];          // Number of errors encountered.
static EllpTokenInfo info;                           // Information about the token.
static bool haveErrors;
static std::string pplastfile;
static std::string lastfile;

enum tokens {
    STRING = EllpStream::CTNEXTOKEN, CHARACTER, INTEGER, FLOAT, IDENTIFIER
};

static EllpWordAssoc tokens[] = {
    { " [a-zA-Z_][a-zA-Z_0-9]*", IDENTIFIER },
    { " [1-9][0-9]*([uU]|[lL])*", INTEGER },            // Decimal integer
    { " 0[xX][0-9a-fA-F]+([uU]|[lL])*", INTEGER },      // Hexadecimal integer
    { " 0[0-7]*([uU]|[lL])*", INTEGER },                        // Octal integer
    { " [0-9]+\\.[0-9]*([eE][-+]?[0-9]+)?", FLOAT },    // <digits>.
    { " [0-9]*\\.[0-9]+([eE][-+]?[0-9]+)?",     FLOAT },    // .<digits>
    { " [0-9]+[eE][-+]?[0-9]+", FLOAT },                        // <digits>e
    { " L?\"(\\\\.|[^\"\n])*\"", STRING },
    { " L?'(\\\\.|[^'\n])*'", CHARACTER },
    { NULL,  0 },
};

static EllpBracket comments[] =
{
    { "//", "\n", EllpStream::COMMENT },        // Single line comment.
    { "/*", "*/", EllpStream::COMMENT },        // Multi line comment.
    { NULL, 0,     0 }
};


//
//
// verror - Handle an error.
//
EllError* verror(EllError::Type type,
                           const std::string& file,
                           int startline, int startcolumn, int endline, int endcolumn,
                           const char *string, va_list ap)
{
    EllError *ep = errors->add(type, file, startline, startcolumn, endline, endcolumn, string, ap);
    ++errorcount[type];                     // Increment the error count for this type.
    if (ep && ep->isError())
        haveErrors = true;                      // Inhibit subsequent processing.

    if (type == EllError::FATAL)
          exit(1);
//        longjmp(fatal, 1);                      // Abort processing.

    return ep;
}

class PP : public Ellp
{
public:
    PP(std::string& name) : Ellp(name, ::macros)
        { }
    EllError* error(EllError::Type type,
                   int startline, int startcolumn, int endline, int endcolumn,
                   const char* format, ...);
    void errorPosition(std::string& buffer, const std::string& file,
                       int startline, int startcolumn, int endline, int endcolumn, bool trailer);
private:
};

//
// error - Handle an error.
//
EllError* PP::error(EllError::Type type,
                       int startline, int startcolumn, int endline, int endcolumn,
                       const char *string, ...)
{
    va_list ap;
    EllError *ep;

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
    Ellp* pp = new PP(file);
    FILE* fp = NULL;
    if (pp == NULL) exit(1);
    if (!pp->setInput(fp)) {
        fprintf(stderr, "can't open %s\n", argv[1]);
    }
    
    pp->getToken(info, Ellp::GETALL);
    lastfile = info.file;                       // Remember the last file for error reporting.
    for (;;) {
        if (lastfile != pplastfile) {
            // Output #line directive if pre-processing.
            pplastfile = lastfile;
            fprintf(stdout, "#line %d \"%s\"\n", info.startline, lastfile.c_str());
        }
        fprintf(stdout, "%s", &info.string);
        pp->getToken(info, Ellp::GETALL);
        lastfile = info.file;                       // Remember the last file for error reporting.
    }
}
