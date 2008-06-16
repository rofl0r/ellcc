/*
 *    pwPP.cc - The pre-processor object.
 *
 *    Copyright (C) 2007, Richard Pennington.
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pwPP.h"
#include "pwOS.h"

namespace pw {

Macro* PP::lookup(std::string& name, int line)
{
    Macro* macp = macros.lookup(name);
     
    if (macp == NULL) {
        return NULL;
    }

    if (macp->undefined) {
        // The last definition has been undefined.
        return false;
    }

    if (macp->type == "file") {
        const std::string& temp = includes ? includes->name : this->name;
        macp->body.erase();

        // Make the file name into a string.
        macp->body = "\"";
        for (size_t i = 0; i < temp.length(); ++i) {
            macp->body += temp[i];
            if (temp[i] == '\\') {
                macp->body += '\\';
            }
        }
        macp->body += '"';
    } else if (macp->type == "line") {
        PPStream *p;
        char buffer[100];

        // Get the current line number.
        if (includes)
            p = includes->pp;
        else
            p = pp;

        sprintf(buffer, "%d", p->startLine());
        macp->body = buffer;
    }

    return macp;
}

int PP::isdefined(std::string& name, int line)
{
    return lookup(name, line) != NULL;
}

bool PP::lookupmacro(std::string& name, int line, Macro*& mpp)
{
    Macro* macp = lookup(name, line);

    if (macp != NULL) {
        mpp = macp;
        return true;
    }

    return false;
}

void PP::definemacro(int line, const std::string& filename, PPStream* data)
{
    definemacro(line, filename, *data, data->type, data->funlike, data->formal, data->body);
}

void PP::definemacro(int line, const std::string& filename, TokenInfo& data,
                        const std::string& type, bool funlike, const pw::array<std::string>& formal,
                        const std::string& body)
{
    int i;
    Macro* macp = lookup(data.string, line);
    pw::Error *ep = NULL;
    std::string buffer;

    if (macp != NULL) {
        int args;
        bool match = true;

        // Macro redefinition. Make sure it matches.

        if (formal.size() != macp->arguments.size()) {
            match = false;

            ep = error(pw::Error::ERROR,
                       line, -1, 0, 0,
                       "Macro \"%s\" redefined with a different number of arguments.", data.string.c_str());
        }
        if (match) {
            for (args = 0; args < macp->arguments.size(); ++args)
                if (macp->arguments[args] != formal[args]) {
                    match = false;
                    ep = error(pw::Error::ERROR,
                               line, -1, 0, 0,
                               "Macro \"%s\" redefined with different argument spelling.", data.string.c_str());
                    pw::Error::info(ep, pw::Error::MORE, "First different argument: \"%s\".", formal[args].c_str());
                    break;
                }
        }
        if (match) {
            if (macp->body != body) {
                match = false;
                ep = error(pw::Error::ERROR,
                           line, -1, 0, 0,
                           "Macro \"%s\" redefined.", data.string.c_str());
            }
        }

        if (!match) {
            if (macp->startline) {
                errorPosition(buffer, macp->file,
                              macp->startline, macp->startcolumn,
                              macp->endline, macp->endcolumn,
                              false);
                pw::Error::info(ep, pw::Error::MORE, "Last definition in %s", buffer.c_str());
            }
        }
        macp->undefined = line;
    }

    macp = new Macro();
    macp->string = data.string;
    macp->type = type;
    macp->file = filename;
    macp->startline = data.startline;
    macp->startcolumn = data.startcolumn;
    macp->endline = data.endline;
    macp->endcolumn = data.endcolumn;
    if (funlike) {
        macp->function = true;
        for (i = 0; i < formal.size(); ++i) {
            macp->arguments[i] = formal[i];
        }
    }
    macp->body = body;
    macros.add(macp);
}

void PP::undefinemacro(std::string& name, int line, int fileline, bool fixed)
{
    Macro* macp = lookup(name, line);

    if (macp != NULL) {
        if (!fixed && macp->type != "defined") {
            error(pw::Error::ERROR,
                  line, -1, 0, 0,
                  "Can't undefine macro \"%s\".", name.c_str());
            return;
        }
        macp->undefined = line;
    }
}

//
// stringgetc - get the next input character from a string
//
int PP::stringgetc()
{
    if (!ip)
        return -1;                              // End of string.
    return *ip++;                               // Get next character.
}

//
// filegetc - get the next input character from a file
//
int PP::filegetc()
{
    if (includes)
        return fgetc(includes->fp);

    return fgetc(fp);
}

//
// addname - add a name to the list of input file names
//
std::string& PP::addname(const std::string& name)
{
    for (int i = 0; i < files.size(); ++i) {
        if (name == files[i])
            return files[i];                    // Already know this name.
    }

    files += name;
    return files[files.size() - 1];
}

//
// addInclude - Add a name to the list of include directories.
//
void PP::addInclude(const std::string& name)
{
    for (int i = 0; i < includedirs.size(); ++i) {
        if (name == includedirs[i]) {
            return;                             // already know this name
        }
    }

    includedirs += name;
}

//
// initializeoptions - set pre-processor options to the default state.
//
void PP::initializeoptions()
{
    // default options (for C)
    options.trigraphs = true;
    options.INTEGER = 0;   
    options.CHARACTER = 0;   
    options.FLOAT = 0;   
    options.STRING = 0;   
    options.IDENTIFIER = 0;   
    options.reservedWords = NULL;   
    options.tokens = NULL;   
    options.comments = NULL;   
}

//
// getOptions - get the current options.
//
void PP::getOptions(Options *op)
{
    *op = options;
}

//
// pspSetOptions - set the current options.
//
void PP::setOptions(Options *op)
{
    options = *op;
    pp->optionsChanged();
}

Error* PP::error(Error::Type type, int sl, int sc, int el, int ec, const char* string, ...)
{
    va_list ap;
    Error *ep;
    va_start(ap, string);
    ep = errors.vadd(type, sl, sc, el, ec, string, ap);
    va_end(ap);
    return ep;
}

//
// PP - Create a pre-processor object.
//
PP::PP(const std::string& name, ErrorList& errors)
    : errors(errors)
{
    time_t timer;
    char *date;
    char month[10], thetime[10];
    int day, year;
    char dates[20], times[20];

    this->name = addname(name);                 // Add the first input name.

    myfp = false;                               // No input source yet.
    fp = NULL;
    sp = NULL;
    includes = NULL;                            // No includes yet.

    initializeoptions();

    pp = new PPStream(*this, &options);

    TokenInfo def;
    pw::array<std::string> formal;                   // An empty parameter list.
    // Define some predefined macros.
    def.string = "__FILE__";
    definemacro(0, "initialization", def, "file", false, formal, "");

    def.string = "__LINE__";
    definemacro(0, "initialization", def, "line", false, formal, "");

    // Get the current time.
    if (time(&timer) == (time_t)-1) {
        timer = 0;                              // Default time.
    }
    date = ctime(&timer);
    sscanf(date + 4, "%s %d %s %d", month, &day, thetime, &year);
    sprintf(dates, "\"%s %.2d %d\"", month, day, year);
    sprintf(times, "\"%s\"", thetime);
    def.string = "__DATE__";
    definemacro(0, "initialization", def, "fixed", false, formal, dates);

    def.string = "__TIME__";
    definemacro(0, "initialization", def, "fixed", false, formal, times);
}

bool PP::setInput(const char *string)
{
    // Use a string for input.
    sp = strdup(string);                        // Set the string pointer.
    ip = sp;                                    // Start of string.
    fp = NULL;                                  // No file.
    pp->setInput(&PP::stringgetc);
    return true;
}

bool PP::setInput(FILE *fp)
{
    sp = NULL;                                  // No string for input.
    if (fp)
        this->fp = fp;
    else {
        myfp = true;
        this->fp = tfopen(name.c_str(), "r");
    }
    if (this->fp == NULL) {
        // error opening file
        return false;
    }

    pp->setInput(&PP::filegetc);
    return true;
}

//
// ~PP - Pre-processor object destructor.
//
PP::~PP()
{
    delete pp;

    // Close and delete include files.
    while (includes) {
        include *tmp = includes;
        pw::fclose(tmp->fp);
        includes = tmp->next;
        delete tmp;
    }

    if (myfp && fp)
        pw::fclose(fp);
    
    delete sp;
}

/* Open an include file.
 */
bool PP::doInclude(PPStream *current)
{
    FILE *fp = NULL;
    std::string name;

    // Remember the line on which the #include occured.
    if (includes == NULL)
        includeline = current->startline;

    // Open a new include file.
    if (!current->sysheader || fullPath(current->string)) {
        fp = tfopen(current->string.c_str(), "r");
        name = current->string;
    }
    if (fp == NULL && !fullPath(current->string)) {
        // Check all paths.
        for (int i = 0; i < includedirs.size(); ++i) {
            name = includedirs[i] + current->string;
            if ((fp = tfopen(name.c_str(), "r")) != NULL) {
                break;
            }
        }
    }

    if (fp == NULL) {
        error(pw::Error::ERROR,
              current->startline, current->startcolumn, current->endline, current->endcolumn,
              "Can't find #include file \"%s\".", current->string.c_str());
        return false;
    }

    include *newp = new include;
    newp->next = includes;
    includes = newp;
    newp->fp = fp;
    newp->name = addname(name);
    // Open new scanning context.
    newp->pp = new PPStream(*this, &options);
    newp->pp->setInput(&PP::filegetc);
    return true;
}

//
//      processnexttoken - get and process the next token
//
void PP::processnexttoken(TokenInfo& tinfo)
{
    PPStream *current;

    for ( ; ; ) {
        if (includes) {
            // processing an include file

            current = includes->pp;
            current->getToken();
            if (current->token == PPStream::ENDOFFILE) {
                // Close this include file...

                pw::fclose(includes->fp);
                delete includes->pp;
                include *incp = includes;
                includes = includes->next;
                delete incp;
                // ...and process next token.
                continue;
            }
        } else {
            // the main file

            current = pp;
            current->getToken();
        }

        if (current->token == PPStream::ENDOFFILE) {
            break;                              // end of main file was reached
        }

        switch (current->token) {
        case PPStream::PDEFINE:
            // define a macro
            if (includes) {
                // define in an include file
                definemacro(includeline, includes->name, current);
            } else {
                // define in the main file
                definemacro(current->startline, name, current);
            }
            continue;

        case PPStream::PUNDEF:
            // undefine a macro
            if (includes) {
                // undefine in an include file
                undefinemacro(current->string, includeline,
                              current->startline, false);
            } else {
                // undefine in the main file
                undefinemacro(current->string, current->startline,
                              current->startline, false);
            }
            continue;

        case PPStream::PINCLUDE:
            // Include a file.
            if (!doInclude(current)) {
                continue;
            }
            break;
        }
        break;
    }

    // Copy the token information.
    tinfo = *current;
}

//
// pspToken - Get the next token.
//
void PP::getToken(Filter filter)
{
    for (;;) {
        processnexttoken(info);

        if (includes) {
            info.file = includes->name;
        } else {
            info.file = name;
        }

        if (info.tokenclass == TokenInfo::TCSKIPPED) {
            continue;
        }

        if (filter == GETALL || info.tokenclass != TokenInfo::TCSPACE) {
            // Return all or non-space tokens.
            return;
        }

        if (filter == GETNL && info.token == PPStream::NL) {
            return;                             // Return newlines.
        }
    }
}

//
// addDefine - Define a macro.
//
void PP::addDefine(const std::string& name, const std::string& value)
{
    TokenInfo def;
    pw::array<std::string> formal;

    def.string = name;
    definemacro(0, "initialization", def, "defined", false, formal, value);
}

//
// pspUndefine - undefine a macro
//
void PP::undefine(std::string& name, bool fixed)
{
    PPStream *p;

    // Get the current line number.
    if (includes)
        p = includes->pp;
    else
        p = pp;

    undefinemacro(name, p->startLine(), pp->startLine(), true);
}

//
// fixedDefine - define a fixed macro
//
void PP::fixedDefine(const std::string& name, const char *value)
{
    TokenInfo def;
    pw::array<std::string> formal;
    std::string body;

    if (value)
        body = value;
    else
        body = "1";
    def.string = name;
    definemacro(0, "initialization", def, "fixed", false, formal, body);
}

//
// process - process a stream
//
bool PP::process()
{
    PPStream *current;

    do {

        if (includes) {
            // Processing an include file.

            current = includes->pp;
            current->getToken();
            if (current->token == PPStream::ENDOFFILE) {
                // Close this include file...

                pw::fclose(includes->fp);
                delete includes->pp;
                include *incp = includes;
                includes = includes->next;
                delete incp;
                // ...and process next token.
                continue;
            }
        } else {
            // The main file.

            current = pp;
            current->getToken();
        }

        if (current->token == PPStream::ENDOFFILE)
            break;                              // end of main file was reached

        switch (current->token) {
        case PPStream::PDEFINE:
            // define a macro
            if (includes) {
                // define in an include file
                definemacro(includeline, includes->name, current);
            } else {
                // define in the main file
                definemacro(current->startline, name, current);
            }
            break;

        case PPStream::PUNDEF:
            // undefine a macro

            if (includes) {
                // undefine in an include file
                undefinemacro(current->string, includeline,
                              current->startline, false);
            } else {
                // undefine in the main file
                undefinemacro(current->string, current->startline,
                              current->startline, false);
            }
            break;

        case PPStream::PINCLUDE:
            // Include a file.
            doInclude(current);
            break;
        }
    } while (current->token != PPStream::ENDOFFILE);

    return true;
}

//
// depends - get the files the source file depends on
//
const pw::array<std::string>& PP::depends()
{
    process();
    return files;
}

};
