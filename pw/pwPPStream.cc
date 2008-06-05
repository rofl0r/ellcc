/*
 *    PPStream.cc - Scanner for the pre-processing object.
 *
 *    Copyright (C) 2008, Richard Pennington.
 */

#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string>
#include "pwArray.h"
#include "pwPP.h"

namespace pw {

#define VA_ARGS "__VA_ARGS__"                   // Name of the var-args "macro".

static pw::Matcher* directives;              // Preprocessing directives.
const WordAssoc PPStream::directivelist[] = // List of preprocessor directives.
{
    { "define",     PPDEFINE},
    { "elif",       PPELIF},
    { "else",       PPELSE},
    { "endif",      PPENDIF},
    { "error",      PPERROR},
    { "if",         PPIF},
    { "ifdef",      PPIFDEF},
    { "ifndef",     PPIFNDEF},
    { "include",    PPINCLUDE},
    { "line",       PPLINE},
    { "pragma",     PPPRAGMA},
    { "undef",      PPUNDEF},
    { NULL,         0}
};

const WordAssoc PPStream::operatorlist[] =         // List of preprocessor operators.
{
    { "#",   POUND},
    { "##",  POUNDPOUND},
    { ",",   COMMA},
    { "(",   LPAREN},
    { ")",   RPAREN},
    { "?",   QUESTION},
    { ":",   COLON},
    { "~",   TILDA},
    { "==",  EQ},
    { "*",   STAR},
    { "/",   SLASH},
    { "%",   PERCENT},
    { "+",   PLUS},
    { "-",   HYPHEN},
    { "&",   AND},
    { "&&",  ANDAND},
    { "^",   CAROT},
    { "|",   BAR},
    { "||",  OROR},
    { "!",   EXCLAMATION},
    { "!=",  NE},
    { "...", ELIPSIS},
    { "<",   LT},
    { "<=",  LE},
    { "<<",  LL},
    { ">",   GT},
    { ">=",  GE},
    { ">>",  RR},
    { NULL,  0},
};

//
// optionsChanged - options have changed, adapt.
//
void PPStream::optionsChanged()
{
    setupStateMachines();
}

//
//      PPStream - Preprocessor stream constructor.
//
PPStream::PPStream(PP& ppsp, Options *options) : psp(ppsp)
{
    const WordAssoc *wp;

    this->options = options;                    // Preprocessor options.
    this->fgetc = NULL;                         // input function
    if (directives == NULL) {
        // Define the preprocessing directive state machine.
        directives = new pw::Matcher("directives", pw::Matcher::CHARSIZE, pw::stateCharName, NULL, 0);
        for (wp = directivelist; wp->word; ++wp) {
            directives->addWord(wp->word, wp->token);
        }
    }

    commentbuffer = NULL;                       // Place to buffer comment start strings.
    setupStateMachines();                       // Set up state machines.

    line = 1;                                   // current source line
    column = 0;                                 // current source column
    first = 0;                                  // no open streams
    nextchar = 0;                               // next character to scan
    hasspace = false;                           // not whitspace yet
    iscomment = false;                          // not a comment
    sawnewline = true;                          // allow a # directive or asm label
    skipping = false;                           // not skipping input
    newlinecount = 0;
    allowheader = false;                        // scan a header name
    neverexpand = false;                        // set for PPIDENT
    noexpand = false;                           // set to inhibit macro expansion
    inpragma = false;                           // set if scanning a pragma.
    conditionals = NULL;                        // no conditionals yet
}

PPStream::~PPStream()
{
    delete commentbuffer;
}

//
// Token state machine helper functions.
//
int PPStream::read(void *arg)
{
    PPStream *pp = (PPStream *)arg;
    pp->readchar();
    return pp->nextchar;
}

void PPStream::save(void *arg, int current)
{
    PPStream *pp = (PPStream *)arg;
    pp->string += current;
}

void PPStream::backup(void *arg, int good, int count)
{
    PPStream *pp = (PPStream *)arg;
    // Restore uneeded characters.
    pp->first = new PPStream::PPStream::Stream(pp->first, "", pp->string.substr(good, pp->string.length()),
                                               pp->nextchar, false);
    pp->string.erase(good, pp->string.length());
    pp->nextchar = 0;
    pp->readchar();
}

void PPStream::setupStateMachines()
{
    int nexttoken = CTNEXTOKEN;
    const WordAssoc *wp;
    int size = 0;
    const Bracket *cp;
    int temp;

    if (!options) {
        return;
    }

    if (options->tokens) {
        if (options->tokens->getMaxvalue() > nexttoken) {
            nexttoken = maxtoken = options->tokens->getMaxvalue();
        }
    }

    if (options->reservedwords) {
        if (options->reservedwords->getMaxvalue() > nexttoken) {
            nexttoken = maxtoken = options->reservedwords->getMaxvalue();
        }
    }

    if (options->tokens) {
        // Add the preprocessor operators.
        for (wp = operatorlist; wp->word; ++wp) {
            int value;

            // See if the token is already defined.
            value = options->tokens->matchWord(wp->word);
            if (value == -1) {
                // Add token with a unique value.
                options->tokens->addWord(wp->word, ++nexttoken);
                operators[wp->token] = nexttoken;   // Remember the new value.
            } else {
                operators[wp->token] = value;       // Remember the pre-defined value.
            }
        }
    }

    if (commentbuffer) {
        delete commentbuffer;
        commentbuffer = NULL;
    }

    temp = 0;
    if (options->comments) {
        // Get a buffer for the longest comment start string plus nul.
        for (cp = options->comments; cp->start; ++cp) {
            temp = strlen(cp->start) + 1;
            if (temp > size) {
                size = temp;
            }
        }
    }

    if (temp) {
        commentbuffer = new char[temp];
    }
}

PPStream::Stream::Stream(Stream *next, const std::string& name, const std::string& body,
                           int nextchar, bool inhibit)
{
    this->body = body;
    this->name = name;
    this->next = next;
    this->oldnextchar = nextchar;
    this->index = 0;
    this->inhibit = inhibit;
}

//
//      getpptoken - get the next non-blank token
//
void PPStream::getpptoken()
{
    do {
        pptoken();
    } while (token == WS || token == COMMENT);
}

//
//      pptoken - get a preprocessor token into buffer
//      return its class
//
#define firstidentifier(ch) (isalpha(ch) || (ch) == '_')
#define nextidentifier(ch) (isalnum(ch) || (ch) == '_')

void PPStream::pptoken()
{
    int mstartline, mstartcolumn;
    int mendline, mendcolumn;
    const Bracket *cp;

    again:
    token = NONE;                               // No token, yet.
    string.clear();

    // newline and pp-token lookahead needed for macro (
    // and string concatenation
    if (newlinecount) {
        --newlinecount;
        token = NL;
        string += '\n';
        goto endpptoken;
    }

    while (nextchar == 0)
        readchar();                             // first character

    iscomment = false;
    commentstartline = nextl;
    commentstartcolumn = nextc;
    if (options->comments) {
        // Look for comments.
        for (cp = options->comments; cp->start; ++cp) {
            const char *p = cp->start;
            char *q = commentbuffer; 

            while (*p && *p == nextchar) {
                // Check for a start of comment string.

                *q++ = *p++;                    // Save the comment start string.
                getnextchar();
            }

            if (*p) {
                // Not a match.
                if (q == commentbuffer) {
                    // No characters matched.
                    continue;
                }
                *q = '\0';
                // Push the unmatched characters back.
                first = new Stream(first, "", commentbuffer, nextchar, false);
                nextchar = 0;
                getnextchar();
                continue;                       // Check the next candidate.
            }

            // Have a valid starting sequence. Look for the end.
            p = cp->end;
            for (;;) {
                if (*p == '\0') {
                    break;                      // Have a comment end.
                }

                getnextchar();
                if (nextchar == -1) {
                    psp.error(pw::Error::ERROR,
                              commentstartline, commentstartcolumn, 0, 0,
                              "Unterminated comment.");
                    return;
                }
                if (*p != nextchar) {
                    p = cp->end;                // Reset end pointer.
                    continue;
                }
                ++p;                            // Check the next character.
            }

            // Found the end of a comment.
            commentendline = nextl;
            commentendcolumn = nextc + 1;
            iscomment = true;
            nextchar = *(p - 1) == '\n' ? '\n' : ' ';    // Replace comment appropriately.
            break;
        }
    }

    if (iscomment) {
        // return comments as separate space token
        startline = commentstartline;
        startcolumn = commentstartcolumn;
        endline = commentendline;
        endcolumn = commentendcolumn;
        token = COMMENT;
        iscomment = false;
        return;
    }

    if (nextchar == ' ') {                      // whitespace
        startline = nextl;
        startcolumn = nextc;
        token = WS;
        string += ' ';
        while (nextchar == ' ')
            readchar();                         // skip blanks
        endline = nextl;
        endcolumn = nextc;
        return;
    }

    // remember the current pos
    startline = nextl;
    startcolumn = nextc;

    if (nextchar < 0) {
        // end of file
        token = ENDOFFILE;
        nextchar = 0;
        goto endpptoken;
    }

    if (nextchar == '\n') {
        string += nextchar;                     // Add character to token.
        nextchar = 0;                           // Force a read the next time through for interactive scanning.
        token = NL;
        goto endpptoken;
    } else if (allowheader && (nextchar == '"' || nextchar == '<')) {
        int end;

        if (nextchar == '<') {
            end = '>';
        } else {
            end = '"';
        }
        readchar();                             // get the next character
        while (   nextchar != '\n'
               && nextchar != -1
               && nextchar != end) {
            string += nextchar;
            readchar();
        }

        if (nextchar != end)
            psp.error(pw::Error::ERROR,
                      startline, startcolumn, endline, endcolumn,
                      "#include name missing trailing \"%c\"..", end);
        else
            readchar();

        sysheader = end == '"' ? false : true;
        token = HEADER;
        endline = nextl;
        endcolumn = nextc;
        return;
    }

    // Look for tokens.
    // Remember potential start of macro call.
    mstartline = mendline = startline;
    mstartcolumn = mendcolumn = startcolumn;
    if (options->tokens) {
        token = options->tokens->matchStream(nextchar, read, save, backup, this);
    } else {
        token = -1;
    }

    if (token == -1) {
        // No token found.
        string += nextchar;                     // Add character to token.
        readchar();
        token = NONE;
    }if (token == options->IDENTIFIER) {
        std::string name;
        std::string body;
        Macro* def;
        int myneednl;

        mendline = nextl;
        mendcolumn = nextc;

        neverexpand = false;
        if (nextchar == NEVEREXPAND) {
            neverexpand = true;
            readchar();
            goto endpptoken;
        }

        if (noexpand || skipping)
            goto endpptoken;                    // don't expand macros

        if (!psp.lookupmacro(string, mstartline, def)) {
            goto endpptoken;                    // not a macro
        }

        // Check to see if stream is active
        Stream *sp;
        bool flag;

        flag = false;
        for (sp = first; sp; sp = sp->next) {
            if (sp->inhibit)
                break;
            if (string == sp->name) {
                flag = true;
                break;
            }
        }

        if (flag) {
            neverexpand = true;
            goto endpptoken;
        }

        // Save the macro name.
        name = string;

        if (def->function) {
            // The next pp token must be '(' to expand.

            myneednl = 0;
            do {
                pptoken();
                if (token == NL)
                    ++myneednl;
            } while (token == WS || token == NL || token == COMMENT);

            if (token != operators[LPAREN]) {
                // Not a function-like macro expansion.
                newlinecount = myneednl;
                first = new Stream(first, "", string, nextchar ? nextchar : -1, false);
                nextchar = 0;
                string = name;
                token = options->IDENTIFIER;
                goto endpptoken;
            }
        }


        body = STARTMACRO;
        body += def->body;
        body += ENDMACRO;

        if (def->function) {
            // function-like macros: get arguments, if any

            bool needspace;
            bool toomany;
            int parenlevel;
            bool oldexpand = noexpand;
            int arguments = 0;
            pw::array<std::string> actual;
            int oldnextchar;
            bool argalloced = false;
            bool hasstring = false;
            bool haspaste = false;
            bool inargument = false;
            int elipsis;

            needspace = false;
            toomany = false;
            parenlevel = 0;
            noexpand = true;

            // Check for an elipsis argument.
            elipsis = -1;
            if (def->arguments.size() && def->arguments[def->arguments.size() - 1] == "...") {
                elipsis = def->arguments.size() - 1;
            }

            // Get the arguments.
            for ( ;; ) {
                pptoken();

                if (token == ENDOFFILE) {
                    psp.error(pw::Error::ERROR,
                              mstartline, mstartcolumn, mendline, mendcolumn,
                              "Call of \"%s\" terminated by end of file.", name.c_str());
                    break;
                }

                if (token == NL || token == COMMENT) {
                    token = WS;
                } else if (token == operators[LPAREN]) {
                    ++parenlevel;
                } else if (parenlevel && token == operators[RPAREN]) {
                    --parenlevel;
                } else if (parenlevel == 0 && (token == operators[COMMA] || token == operators[RPAREN])) {
                    // save this argument
                    if (!argalloced && def->arguments.size()) {
                        // Not allocted yet.
                        argalloced = true;
                    }
                    if (argalloced && token == operators[COMMA]) {
                        if (needspace) {
                            needspace = false;
                            actual[arguments] += ' ';
                        }

                        if (elipsis == arguments) {
                            // Save commas in elipsis arguments.
                            actual[arguments] += ',';
                        }
                    }

                    if (arguments >= def->arguments.size()) {
                        if (arguments) {
                            if (!toomany) {
                                psp.error(pw::Error::ERROR,
                                          startline, startcolumn, endline, endcolumn,
                                          "Call of \"%s\" has too many arguments.", name.c_str());
                            }
                            toomany = true;
                        }
                    } else if (elipsis != arguments) {
                        ++arguments;            // Count arguments.
                        argalloced = false;
                    }

                    if (token == operators[RPAREN]) {
                        // remember end of macro call
                        mendline = endline;
                        mendcolumn = endcolumn;
                        break;
                    }
                    continue;
                }

                if (token != WS) {
                    // Save argument string.
                    if (!argalloced) {
                        argalloced = true;
                    }
                    if (needspace) {
                        needspace = false;
                        actual[arguments] += ' ';
                    }

                    actual[arguments] += string;
                } else if (argalloced && actual[arguments].length()) {
                    needspace = true;
                }
            }

            if (elipsis >= 0) {
                // Count the last argument.
                ++arguments;
            }

            if (arguments < def->arguments.size()) {
                pw::Error *ep = NULL;
                std::string buffer;

                ep = psp.error(pw::Error::ERROR,
                               startline, startcolumn, endline, endcolumn,
                               "Call of \"%s\" has too few arguments %d.", name.c_str(), arguments);
                pw::Error::info(ep, pw::Error::MORE, "\"%s\" is defined with %d arguments.",
                              name.c_str(), def->arguments.size());
                psp.errorPosition(buffer, def->file,
                                  def->startline, def->startcolumn,
                                  def->endline, def->endcolumn,
                                  false);
                pw::Error::info(ep, pw::Error::MORE, "Last definition in %s", buffer.c_str());
            }

            oldnextchar = nextchar;
            noexpand = true;
            // need PPEOF
            first = new Stream(first, "", body, -1, false);
            nextchar = 0;
            needspace = false;
            hasstring = false;
            haspaste = false;

            // get the body, expanding arguments
            body = STARTMACRO;

            int oldnext = 0;
            for ( ;; ) {
                pptoken();
                if (string[0] == PASTE) {
                    // Remove the paste operator.
                    haspaste = true;
                    continue;
                }

                if (token == ENDOFFILE) {
                    // End of body or argument.
                    if (inargument) {
                        nextchar = oldnext;
                        if (needspace) {
                            // a space preceeded
                            body += ' ';
                            needspace = false;
                        }
                        // close quote
                        if (hasstring) {
                            body += '"';
                            hasstring = false;
                        }
                        inargument = false;
                        noexpand = true;
                        continue;
                    } else {
                        nextchar = oldnextchar;
                        break;
                    }
                }

                if (hasstring && (token == options->CHARACTER || token == options->STRING)) {
                    if (needspace) {
                        // a space preceeded
                        body += ' ';
                        needspace = false;
                    }

                    for (size_t i = 0; i < string.length(); ++i) {
                        if (string[i] == '"' || string[i] == '\\')
                            body += '\\';

                        body += string[i];
                    }
                    continue;
                }

                if (token == NL || token == COMMENT) {
                    // newlines and comments become whitespace
                    token = WS;
                }


                if (token == WS) {
                    needspace = true;
                    continue;
                }

                if (string[0] == STRINGIZE) {
                    // Set up the stringize operator.
                    hasstring = true;
                    if (needspace) {
                        // a space preceeded
                        body += ' ';
                        needspace = false;
                    }

                    pptoken();                  // Get the following identifier.
                }

                if (inargument || token != options->IDENTIFIER) {
                    // everything but identifiers
                    saveit:                     // non-args come here
                    if (needspace) {
                        // a space preceeded
                        body += ' ';
                        needspace = false;
                    }

                    // add the pp token
                    body += string;
                    if (token == options->IDENTIFIER
                        && (neverexpand || string == name)) {
                        body += NEVEREXPAND;
                    }
                    continue;
                }

                int found;              // # of found argument.
                if (string == VA_ARGS) {
                    // This substitutes for ...
                    if (elipsis < 0) {
                        psp.error(pw::Error::ERROR,
                                  startline, startcolumn, endline, endcolumn,
                                  "\"%s\" in a macro body with no \"...\" argument.",
                                  VA_ARGS);
                        goto saveit;
                    } else {
                        // Use the elipsis argument.
                        found = elipsis;
                    }
                } else {
                    // Check for macro arguments.
                    for (found = 0; found < arguments && found < def->arguments.size(); ++found) {
                        if (def->arguments[found] == string)
                            break;
                    }

                    if (found >= arguments) {
                        goto saveit;            // Just an identifier.
                    }
                }

                // This is an argument.
                inargument = true;

                oldnext = nextchar;

                // Expand arguments.
                noexpand = haspaste;
                haspaste = false;
                first = new Stream(first, "", actual[found], -1, false);
                nextchar = 0;
                if (hasstring) {
                    if (needspace) {
                        // a space preceeded
                        body += ' ';
                        needspace = false;
                    }

                    body += '"';                // Opening quote.
                }
            }

            body += ENDMACRO;
            noexpand = oldexpand;
        }

        // expand macro
        first = new Stream(first, name, body, nextchar, false);
        nextchar = 0;
        string = name;
        goto again;
    }

    endpptoken:
    // mark end of token
    endline = nextl;
    endcolumn = nextc;
}

//
//      getToken - Get the next token.
//
void PPStream::getToken(TokenInfo& data)
{
    getToken();
    data = *this;
}

//
//      gettoken - Get the next token.
//
void PPStream::getToken()
{
    again:
    pptoken();
    tokenclass = TokenInfo::TCNONE;           // No class yet.
    if (token == ENDOFFILE) {
        sawnewline = true;                      // have seen a new line
        while (conditionals) {
            Conditional *cp = conditionals;

            psp.error(pw::Error::ERROR,
                      cp->line, cp->column, 0, 0,
                      "Unterminated #if or #ifdef.");
            conditionals = cp->next;
            delete cp;
        }
        token = ENDOFFILE;
        return;                                 // Hit end of file.
    }

    if (token == COMMENT) {
        if (skipping)
            goto endofline;
        tokenclass = TokenInfo::TCSPACE;
        token = COMMENT;
        return;
    }

    if (token == NL) {                          // newline
        sawnewline = true;                      // Have seen a new line.
        tokenclass = TokenInfo::TCSPACE;
        if (inpragma) {
            // Leaving a pragma.
            inpragma = false;
            noexpand = false;
        }
        token = NL;
        return;
    }

    if (token == WS) {                          // whitespace
        if (skipping)
            goto again;
        tokenclass = TokenInfo::TCSPACE;
        token = WS;
        return;
    }

    if (sawnewline && token == operators[POUND]) {
        PPDirectives ppdirective;
        int pstartline, pstartcolumn;

        // This is a preprocessor directive.
        pstartline = startline;
        pstartcolumn = startcolumn;
        tokenclass = TokenInfo::TCPPDIRECTIVE;
        noexpand = true;
        getpptoken();
        if (token == NL) {
            startline = pstartline;
            startcolumn = pstartcolumn;
            token = POTHER;
            return;
        }

        if (   token != options->IDENTIFIER
               || (ppdirective = isppdirective()) == PPDNONE) {
            if (!skipping)
                psp.error(pw::Error::ERROR,
                          startline, startcolumn, endline, endcolumn,
                          "Missing preprocessor directive.");
            startline = pstartline;
            startcolumn = pstartcolumn;
            tokenclass = TokenInfo::TCSKIPPED;
            noexpand = false;
            token = POTHER;
            return;
        }

        switch (ppdirective) {
        case PPDEFINE:                          // #define
            {
                std::string name;
                int arguments = 0;
                int i;
                bool needspace;
                bool haspaste;
                bool hasstring;
                bool haselipsis = false;

                if (skipping)
                    goto endofline;

                formal.truncate();
                body.erase();
                funlike = false;

                getpptoken();
                if (token != options->IDENTIFIER) {
                    psp.error(pw::Error::ERROR,
                              startline, startcolumn, endline, endcolumn,
                              "#define missing identifier.");
                    goto endofline;
                }
                name = string;

                if (nextchar == '(') {
                    // function like macro

                    funlike = true;
                    pptoken();                  // eat the open paren
                    for ( ;; ) {
                        getpptoken();
                        if (token == NL) {
                            psp.error(pw::Error::ERROR,
                                      startline, startcolumn, endline, endcolumn,
                                      "#define missing closing paren.");
                            goto nullbody;
                        }
                        if (token == operators[RPAREN]) {
                            break;
                        }
                        if (arguments > 0) {
                            if (token != operators[COMMA]) {
                                psp.error(pw::Error::ERROR,
                                          startline, startcolumn, endline, endcolumn,
                                          "#define missing comma.");
                            } else
                                getpptoken();
                        }

                        if (token == options->IDENTIFIER) {
                            if (haselipsis) {
                                psp.error(pw::Error::ERROR,
                                          startline, startcolumn, endline, endcolumn,
                                          "Identifier #define argument follows an \"...\".");
                                continue;
                            }
                        } else if (token == operators[ELIPSIS]) {
                            if (haselipsis) {
                                psp.error(pw::Error::ERROR,
                                          startline, startcolumn, endline, endcolumn,
                                          "#define argument list has more than one \"...\".");
                                continue;
                            }
                            haselipsis = true;
                        } else {
                            psp.error(pw::Error::ERROR,
                                      startline, startcolumn, endline, endcolumn,
                                      "#define argument (%s) is not an identifier or \"...\".", string.c_str());
                            continue;
                        }

                        // Have an argument name.

                        // Check for duplicate name.
                        for (i=0; i < arguments; ++i) {
                            if (string == formal[i]) {
                                psp.error(pw::Error::ERROR,
                                          startline, startcolumn, endline, endcolumn,
                                          "Argument \"%s\" not unique in macro definition.", string.c_str());
                            }
                        }

                        if (string == VA_ARGS) {
                            psp.error(pw::Error::ERROR,
                                      startline, startcolumn, endline, endcolumn,
                                      "\"%s\" not allowed in macro parameter list.", VA_ARGS);
                        }
                        // remember argument
                        formal[arguments] = string;
                        ++arguments;
                    }
                }

                // get the body
                getpptoken();                   // skip leading whitespace
                nullbody:
                body.erase();
                needspace = false;
                hasstring = false;
                haspaste = false;
                while (token != NL && token != ENDOFFILE) {
                    // get preprocessor tokens and save them 

                    if (token == operators[POUNDPOUND]) {
                        if (hasstring) {
                            // This wasn't a stringize operator.
                            body += '#';
                            hasstring = false;
                        }

                        if (body.length() == 0)
                            psp.error(pw::Error::ERROR,
                                      startline, startcolumn, endline, endcolumn,
                                      "\"##\" operator illegal at start of replacement list.");
                        else {
                            // Add a paste operator.
                            needspace = false;
                            haspaste = true;
                        }
                    } else if (token == operators[POUND]) {
                        if (haspaste) {
                            body += PASTE;
                            haspaste = false;
                            needspace = false;
                        }

                        hasstring = true;
                        if (needspace) {
                            body += ' ';
                            needspace = false;
                        }
                    } else if (token != WS && token != COMMENT) {
                        if (haspaste) {
                            body += PASTE;
                            haspaste = false;
                            needspace = false;
                        }

                        if (token == options->IDENTIFIER && hasstring) {
                            // Check for an argument.
                            if (haselipsis && string == VA_ARGS) {
                                // Stingize the variable arguments.
                                body += STRINGIZE;
                                hasstring = false;
                                needspace = false;
                            } else {
                                for (int i = 0; i < arguments; ++i) {
                                    if (string == formal[i]) {
                                        // This is an argument. Stringize.

                                        body += STRINGIZE;
                                        hasstring = false;
                                        needspace = false;
                                        break;
                                    }
                                }
                            }
                        }

                        if (hasstring) {
                            // This wasn't a stringize operator.
                            body += '#';
                            hasstring = false;
                        }

                        if (needspace) {
                            body += ' ';
                            needspace = false;
                        }


                        body += string;
                    } else if (!haspaste && !hasstring && body.length()) {
                        needspace = true;
                    }
                    pptoken();
                }

                if (hasstring) {
                    // This wasn't a stringize operator.
                    body += '#';
                }

                if (haspaste) {
                    psp.error(pw::Error::ERROR,
                              startline, startcolumn, endline, endcolumn,
                              "\"##\" operator illegal at end of replacement list.");
                }

                string = name;
                type = "defined";
                noexpand = false;
                startline = pstartline;
                startcolumn = pstartcolumn;
                token = PDEFINE;
                return;
            }

        case PPELSE:
            if (!conditionals) {
                psp.error(pw::Error::ERROR,
                          startline, startcolumn, endline, endcolumn,
                          "#else with no matching #if or #ifdef.");
            } else if (conditionals->haselse) {
                psp.error(pw::Error::ERROR,
                          startline, startcolumn, endline, endcolumn,
                          "Multiple #elses in conditional.");
            } else {
                conditionals->haselse = true;
                if (!conditionals->skipping) {
                    if (skipping) {
                        tokenclass = TokenInfo::TCSKIPPED;
                        pstartline = conditionals->skipline;
                        pstartcolumn = conditionals->skipcolumn;
                    } else {
                        conditionals->skipline = pstartline;
                        conditionals->skipcolumn = pstartcolumn;
                    }
                    skipping = conditionals->hastruepart;
                }
            }

            if (conditionals && conditionals->skipping)
                goto endofline;                 // skipping enclosing
            pptoken();
            startline = pstartline;
            startcolumn = pstartcolumn;
            noexpand = false;
            token = POTHER;
            return;                             // Only a newline or whitespace can follow.

        case PPENDIF:
            {
                Conditional *cp;

                if (!(cp = conditionals)) {
                    psp.error(pw::Error::ERROR,
                              startline, startcolumn, endline, endcolumn,
                              "#endif with no matching #if or #ifdef.");
                    pptoken();
                } else {
                    conditionals = cp->next;
                    if (skipping) {
                        // was skipping
                        startline = cp->skipline;
                        startcolumn = cp->skipcolumn;
                        tokenclass = TokenInfo::TCSKIPPED;
                    }
                    skipping = cp->skipping;
                    delete cp;
                    if (skipping)
                        goto endofline;
                    pptoken();
                }
                startline = pstartline;
                startcolumn = pstartcolumn;
                noexpand = false;
                token = POTHER;
                return;                         // Only a newline or whitespace can follow.
            }

        case PPELIF:
            if (!conditionals) {
                psp.error(pw::Error::ERROR,
                          startline, startcolumn, endline, endcolumn,
                          "#elif with no matching #if or #ifdef.");
                goto leave;
            } else if (conditionals->haselse) {
                psp.error(pw::Error::ERROR,
                          startline, startcolumn, endline, endcolumn,
                          "#elif follows #else in conditional.");
            }
            if (conditionals->skipping)
                goto endofline;                 // skipping enclosing
            if (conditionals->hastruepart) {
                if (!skipping) {
                    conditionals->skipline = pstartline;
                    conditionals->skipcolumn = pstartcolumn;
                }
                skipping = true;
            } else {
                if (skipping) {
                    pstartline = conditionals->skipline;
                    pstartcolumn = conditionals->skipcolumn;
                    tokenclass = TokenInfo::TCSKIPPED;
                    skipping = false;
                }
                noexpand = false;
                getpptoken();                   // eat the elif
                skipping = !conditionalexpr();
            }

            if (!skipping) {
                conditionals->hastruepart = true;
                conditionals->line = pstartline;
                conditionals->column = pstartcolumn;
            }
            leave:
            startline = pstartline;
            startcolumn = pstartcolumn;
            noexpand = false;
            token = POTHER;
            return;

        case PPIF:
        case PPIFDEF:
        case PPIFNDEF:
            {
                Conditional *cp;

                // make a new conditional context
                cp = new Conditional;
                cp->next = conditionals;
                cp->hastruepart = false;
                cp->haselse = false;
                cp->line = startline;
                cp->column = startcolumn;
                cp->skipping = skipping;
                conditionals = cp;
                if (skipping)
                    goto endofline;             // just ignore
                if (ppdirective == PPIF) {
                    noexpand = false;
                    getpptoken();               // eat the directive
                    if (!conditionalexpr())
                        skipping = true;
                } else {
                    // #ifdef or #ifndef

                    getpptoken();
                    noexpand = false;
                    if (token != options->IDENTIFIER) {
                        psp.error(pw::Error::ERROR,
                                  startline, startcolumn, endline, endcolumn,
                                  "#if%sdef missing identifier.",
                                  ppdirective == PPIFDEF ? "" : "n");
                        break;
                    }

                    if (psp.isdefined(string, startline)) {
                        if (ppdirective == PPIFNDEF)
                            skipping = true;
                    } else {
                        if (ppdirective == PPIFDEF)
                            skipping = true;
                    }
                    pptoken();                  // eat the identifier
                }

                cp->hastruepart = !skipping;
                if (!skipping) {
                    startline = pstartline;
                    startcolumn = pstartcolumn;
                    token = POTHER;
                    return;
                }

                cp->skipline = pstartline;
                cp->skipcolumn = pstartcolumn;
                break;
            }

        case PPINCLUDE:
            if (skipping)
                goto endofline;

            allowheader = true;
            noexpand = false;
            getpptoken();
            allowheader = false;
            if (token != HEADER) {
                psp.error(pw::Error::ERROR,
                          startline, startcolumn, endline, endcolumn,
                          "#include missing filename, given \"%s\"", string.c_str());
                goto endofline;
            }
            startline = pstartline;
            startcolumn = pstartcolumn;
            token = PINCLUDE;
            return;

        case PPUNDEF:   
            if (skipping)
                goto endofline;

            getpptoken();
            noexpand = false;
            if (token != options->IDENTIFIER) {
                psp.error(pw::Error::ERROR,
                          startline, startcolumn, endline, endcolumn,
                          "#undef missing identifier.");
                goto endofline;
            }
            startline = pstartline;
            startcolumn = pstartcolumn;
            token = PUNDEF;
            return;

        case PPERROR:
            {
                std::string errorstring;
                int estartline, estartcolumn;
                bool needspace = false;

                estartline = startline;
                estartcolumn = startcolumn;

                if (skipping)
                    goto endofline;
                noexpand = false;
                getpptoken();
                while (token != NL) {
                    // get preprocessor tokens and save them

                    if (token != WS && token != COMMENT) {
                        if (needspace)
                            errorstring += ' ';
                        needspace = false;
                        errorstring += string;
                    } else
                        needspace = true;
                    pptoken();
                }
                if (errorstring.length()) {
                    psp.error(pw::Error::ERROR,
                              estartline, estartcolumn, endline, endcolumn,
                              "%s.", errorstring.c_str());
                } else {
                    psp.error(pw::Error::ERROR,
                              estartline, estartcolumn, endline, endcolumn,
                              "#error.");
                }

                break;
            }

        case PPPRAGMA:
            if (skipping)
                goto endofline;

            // Return pragma tokens without expansion until the next newline.
            inpragma = true;
            token = PPRAGMA;
            return;

        case PPLINE:
            {
                if (skipping)
                    goto endofline;
                noexpand = false;
                getpptoken();
                if (token != options->INTEGER) {
                    psp.error(pw::Error::ERROR,
                              startline, startcolumn, endline, endcolumn,
                              "Digit sequence required in #line.");
                    goto endofline;
                }

                unsigned long line = 0;
                size_t index = 0;
                while (index < string.length() && isdigit(string[index])) {
                    line *= 10;

                    line += string[index++] - '0';
                }

                if (index < string.length() - 1) {
                    psp.error(pw::Error::ERROR,
                              startline, startcolumn,
                              endline, endcolumn,
                              "Non-digit encountered in #line.");
                    goto endofline;
                }

                if (line == 0 || line > 2147483647) {
                    // These limits are defined in ISO/IEC 9899.
                    psp.error(pw::Error::ERROR,
                              startline, startcolumn, endline, endcolumn,
                              "#line digit sequence should be greater than zero and less than 2147483647.");
                }
                allowheader = true;
                getpptoken();
                allowheader = false;
                if (token == NL)
                    string.erase();
                else if (   token != HEADER
                            || string[0] == 'L'
                            || sysheader) {
                    psp.error(pw::Error::ERROR,
                              startline, startcolumn,
                              endline, endcolumn,
                              "String literal required in #line.");
                    goto endofline;
                }
                linevalue = line;
                startline = pstartline;
                startcolumn = pstartcolumn;
                token = PLINE;
                return;
            }
        default:
            break;
        }

        while (token == WS || token == COMMENT)
            pptoken();                          // skip whitespace

        if (token != NL) {
            psp.error(pw::Error::ERROR,
                      startline, startcolumn, endline, endcolumn,
                      "Badly formed preprocessor directive.");
            endofline:
            noexpand = false;
            while (token != NL && token != ENDOFFILE)
                pptoken();
            if (token == ENDOFFILE) {
                if (skipping) {
                }
                skipping = false;
                token = ENDOFFILE;
                return;                         // EOF encountered
            }
        }
        tokenclass = TokenInfo::TCSPACE;
        noexpand = false;
        sawnewline = true;                      // have seen a new line
        string = "\n";
        token = NL;
        return;
    }

    sawnewline = false;                         // preprocessor directive not allowed until
    // next newline

    if (skipping)
        goto endofline;                         // skipping conditional stuff

    tokenclass = TokenInfo::TCOPERATOR;
    if (token == options->INTEGER || token == options->FLOAT) {
        tokenclass = TokenInfo::TCCONSTANT;
        return;
    }

    if (token == options->CHARACTER) {
        tokenclass = TokenInfo::TCCONSTANT;
        token = options->CHARACTER;
        return;
    }

    if (token == options->STRING) {
        std::string thisString;
        int sstartline, sstartcolumn;
        int sendline, sendcolumn;
        int myneednl;

        sstartline = startline;
        sstartcolumn = startcolumn;

        do {
            // Combine adjacent strings.

            thisString += string;
            sendline = endline;
            sendcolumn = endcolumn;

            // Find the next token.
            myneednl = 0;
            do {
                pptoken();
                if (token == NL)
                    ++myneednl;
            }
            while (token == WS || token == COMMENT || token == NL);

        } while (token == options->STRING);

        // Push the next token back.
        newlinecount = myneednl;
        first = new Stream(first, "", string, nextchar, false);
        nextchar = 0;
        string = thisString;
        token = options->STRING;
        tokenclass = TokenInfo::TCSTRING;
        startline = sstartline;
        startcolumn = sstartcolumn;
        endline = sendline;
        endcolumn = sendcolumn;

        token = options->STRING;
        return;
    }

    if (token == options->IDENTIFIER) {
        int reserved;

        // check for a reserved word
        if ((reserved = isreserved()) >= 0) {
            tokenclass = TokenInfo::TCRESERVED;
            token = reserved;
            return;
        }

        tokenclass = TokenInfo::TCIDENTIFIER;
        token = options->IDENTIFIER;
        return;
    }

    if (token > maxtoken) {
        // This is a pre-processor-only token.
        token = NONE;
    }
}

int PPStream::isreserved()
{
    if (!options->reservedwords) {
        return -1;
    }

    return options->reservedwords->matchWord(string);
}

PPStream::PPDirectives PPStream::isppdirective() {
    return (PPDirectives)directives->matchWord(string);
}

bool PPStream::primary(long *value)
{
    char *p;

    *value = 0;

    if (token == WS || token == COMMENT) {
        pptoken();
    }

    if (token == options->IDENTIFIER) {
        if (string == std::string("defined")) {
            // this is the defined operator
            bool hasparen = false;

            noexpand = true;
            getpptoken();
            if (token == operators[LPAREN]) {
                hasparen = true;
                getpptoken();
            }
            noexpand = false;
            if (token != options->IDENTIFIER) {
                psp.error(pw::Error::ERROR,
                          startline, startcolumn, endline, endcolumn,
                          "The argument of \"defined\" must be an identifier.");
            } else {
                if (psp.isdefined(string, startline)) {
                    *value = 1;
                }
            }

            if (hasparen) {
                getpptoken();
                if (token != operators[RPAREN]) {
                    psp.error(pw::Error::ERROR,
                              startline, startcolumn, endline, endcolumn,
                              "\"defined\" missing closing paren.");
                }
            }
        }
    } else if (token == options->CHARACTER) {
        p = convertcharacter(value, string);

        if (p) {
            psp.error(pw::Error::ERROR,
                      startline, startcolumn, endline, endcolumn,
                      "%s.", p);
            return false;
        }
    } else if (token == options->FLOAT) {
        psp.error(pw::Error::ERROR,
                  startline, startcolumn, endline, endcolumn,
                  "Floating constant not allowed in #if expression.");
        return false;
    } else if (token == options->INTEGER) {
        p = convertnumber(value, string);
        if (p) {
            psp.error(pw::Error::ERROR,
                      startline, startcolumn, endline, endcolumn,
                      "%s.", p);
            return false;
        }
    } else if (token == options->STRING) {
        psp.error(pw::Error::ERROR,
                  startline, startcolumn, endline, endcolumn,
                  "String not allowed in #if expression.");
        return false;
    } else if (token == NL) {
        psp.error(pw::Error::ERROR,
                  startline, startcolumn, endline, endcolumn,
                  "Newline encountered before the end of an #if expression.");

        return false;
    } else if (token == operators[LPAREN]) {
        // open paren
        getpptoken();
        if (!expression(value))
            return false;
        if (token != operators[RPAREN]) {
            psp.error(pw::Error::ERROR,
                      startline, startcolumn, endline, endcolumn,
                      "')' missing in #if expression.");
            return false;
        }
    } else {
        psp.error(pw::Error::ERROR,
                  startline, startcolumn, endline, endcolumn,
                  "Bad token (%s) in #if expression.", string.c_str());
        return false;
    }

    getpptoken();
    return true;
}

bool PPStream::unaryexpression(long *value)
{
    int oper = token;

    if (oper == operators[PLUS]
        || oper == operators[HYPHEN]
        || oper == operators[TILDA]
        || oper == operators[EXCLAMATION]) {
        getpptoken();
        if (!unaryexpression(value))
            return false;
        if (oper == operators[PLUS]) {
            return true;
        } else if (oper == operators[HYPHEN]) {
            *value = -*value;
            return true;
        } else if (oper == operators[TILDA]) {
            *value = ~*value;
            return true;
        } else if (oper == operators[EXCLAMATION]) {
            if (*value == 0)
                *value = 1;
            else
                *value = 0;
            return true;
        }
    }
    return primary(value);
}

bool PPStream::multiplicativeexpression(long *value)
{
    long value1;
    int i;

    if (!unaryexpression(value))
        return false;
    while ((i = token) == operators[STAR] || i == operators[SLASH] || i == operators[PERCENT]) {
        getpptoken();
        if (!unaryexpression(&value1))
            return false;

        if (i == operators[STAR])
            *value *= value1;
        else {
            // divide or mod
            if (i == operators[PERCENT])
                *value %= value1;
            else
                *value /= value1;
        }
    }
    return true;
}

bool PPStream::additiveexpression(long *value)
{
    long value1;
    int i;

    if (!multiplicativeexpression(value))
        return false;
    while ((i = token) == operators[PLUS] || i == operators[HYPHEN]) {
        getpptoken();
        if (!multiplicativeexpression(&value1))
            return false;
        if (i == operators[PLUS])
            *value += value1;
        else
            *value -= value1;
    }
    return true;
}

bool PPStream::shiftexpression(long *value)
{
    long value1;
    int i;

    if (!additiveexpression(value))
        return false;
    while ((i = token) == operators[LL] || i == operators[RR]) {
        getpptoken();
        if (!additiveexpression(&value1))
            return false;
        if (i == operators[LL])
            *value <<= value1;
        else
            *value >>= value1;
    }
    return true;
}

bool PPStream::relationalexpression(long *value)
{
    long value1;
    int i;
    int cmp;

    if (!shiftexpression(value))
        return false;
    while ((i = token) == operators[LT] || i == operators[GT] || i == operators[LE] || i == operators[GE]) {
        getpptoken();
        if (!shiftexpression(&value1))
            return false;
        cmp = *value - value1;
        if ((cmp == 0 && (i == operators[LE] || i == operators[GE]))
            || (cmp < 0 && (i == operators[LT] || i == operators[LE]))
            || (cmp > 0 && (i == operators[GT] || i == operators[GE]))) {
            cmp = 1;
        } else {
            cmp = 0;
        }

        *value = cmp;
    }
    return true;
}

bool PPStream::equalityexpression(long *value)
{
    long value1;
    int i;
    int cmp;

    if (!relationalexpression(value))
        return false;
    while ((i = token) == operators[EQ] || i == operators[NE]) {
        getpptoken();
        if (!relationalexpression(&value1))
            return false;
        cmp = !(*value - value1);
        if (i == operators[NE])
            cmp = !cmp;
        *value = cmp;
    }
    return true;
}

bool PPStream::ANDexpression(long *value)
{
    long value1;

    if (!equalityexpression(value))
        return false;
    while (token == operators[AND]) {
        getpptoken();
        if (!equalityexpression(&value1))
            return false;
        *value &= value1;
    }
    return true;
}

bool PPStream::exclusiveORexpression(long *value)
{
    long value1;

    if (!ANDexpression(value))
        return false;
    while (token == operators[CAROT]) {
        getpptoken();
        if (!ANDexpression(&value1))
            return false;
        *value ^= value1;
    }
    return true;
}

bool PPStream::inclusiveORexpression(long *value)
{
    long value1;

    if (!exclusiveORexpression(value))
        return false;
    while (token == operators[BAR]) {
        getpptoken();
        if (!exclusiveORexpression(&value1))
            return false;
        *value |= value1;
    }
    return true;
}

bool PPStream::logicalANDexpression(long *value)
{
    long value1;

    if (!inclusiveORexpression(value))
        return false;
    while (token == operators[ANDAND]) {
        getpptoken();
        if (!inclusiveORexpression(&value1))
            return false;
        if (*value != 0 && value1 != 0)
            *value = 1;
        else
            *value = 0;
    }
    return true;
}

bool PPStream::logicalORexpression(long *value)
{
    long value1;

    if (!logicalANDexpression(value))
        return false;
    while (token == operators[OROR]) {
        getpptoken();
        if (!logicalANDexpression(&value1))
            return false;
        if (*value != 0 || value1 != 0)
            *value = 1;
        else
            *value = 0;
    }
    return true;
}

bool PPStream::conditionalexpression(long *value)
{
    long value1, value2;

    if (!logicalORexpression(value))
        return false;
    if (token != operators[QUESTION]) {
        return true;
    }
    getpptoken();
    if (!expression(&value1))
        return false;
    if (token != operators[COLON]) {
        psp.error(pw::Error::ERROR,
                  startline, startcolumn, endline, endcolumn,
                  "':' missing in #if expression.");
        return false;
    }
    getpptoken();
    if (!conditionalexpression(&value2))
        return false;
    if (value == 0)
        *value = value2;
    else
        *value = value1;
    return true;
}

bool PPStream::expression(long *value)
{
    if (!conditionalexpression(value))
        return false;

    while (token == operators[COMMA]) {
        getpptoken();
        if (!conditionalexpression(value))
            return false;
    }
    return true;
}

//
//      conditionalexpr - evaluate a preprocessor conditional expression
//
bool PPStream::conditionalexpr()
{
    long value;

    while (token == WS || token == COMMENT)
        pptoken();

    if (!conditionalexpression(&value)) {
        // Error occured: skip following tokens.
        while (token != NL && token != ENDOFFILE)
            pptoken();
        value = 1;
    }
    while (token == WS || token == COMMENT)
        pptoken();
    return(int)value != 0;
}

//
//      convertcharacter - convert the token buffer to a character
//
char *PPStream::convertcharacter(long *value, const std::string& string)
{
    int wide;
    int index;

    index=0;
    wide = string[0] == 'L';
    if (wide) {
        ++index;                                // skip the 'L'
    }

    ++index;                                    // skip the opening quote

    *value = 0;
    while (string[index] != '\'') {
        // XXX escape sequences
        *value <<= 8;
        // XXX signedness of characters
        *value += string[index++];
    }
    return 0;                                   // No error.
}

//
//      convertnumber - convert the string to a number
//
char *PPStream::convertnumber(long *value, const std::string& string)
{
    int base;
    bool islong, isunsigned;
    bool octalorhex = false;
    int index = 0;

    islong = isunsigned = false;

    // this is an integer constant
    base = 10;
    if (string[0] == '0') {
        base = 8;
        if (string[++index] == 'x' || string[index] == 'X') {
            base = 16;
            ++index;
        }
        octalorhex = true;
    }

    *value = 0;
    while (string[index]) {
        int digit = string[index];

        if (islower(digit))
            digit = toupper(digit);
        if (isdigit(digit))
            digit -= '0';
        else if (digit >= 'A' && digit <= 'F')
            digit = digit - 'A' + 10;
        else
            break;
        if (digit >= base)
            goto baddigit;

        *value *= base;
        if (digit)
            *value += digit;
        ++index;
    }
    while (string[index] == 'l' || string[index] == 'L' || string[index] == 'u' || string[index] == 'U') {
        bool haserror = false;

        if (tolower(string[index]) == 'u') {
            if (isunsigned)
                haserror = true;
            isunsigned = true;
        } else {
            if (islong)
                haserror = true;
            islong = true;
        }
        if (haserror)
            return "Duplicate suffixes in constant";
        ++index;
    }

    if (string[index]) {
        baddigit:
        return "Illegal digit in numeric constant";
    }

    return 0;
}

//
//      getnextchar - low level character reader
//
void PPStream::getnextchar()
{
    again:
    if (first) {
        Stream *t;

        if (first->index >= first->body.length()) {
            nextchar = -1;
        } else {
            nextchar = first->body[first->index++];
        }

        if (nextchar == STARTMACRO) {
            goto again;
        }
        if (nextchar == ENDMACRO) {
            goto again;
        }

        if (nextchar != -1)
            return;

        t = first;
        first = first->next;
        nextchar = t->oldnextchar;
        delete t;
        return;
    }

    nextchar = (psp.*fgetc)();

    if (nextchar == EOF) {
        // eof encountered

        nextchar = -1;
        return;
    }

    // keep track of lines and columns
    nextl = line;
    nextc = column;

    switch (nextchar) {
    case '\f':                                  // XXX for now
    case '\v':                                  // XXX for now
        // ++line;
        // column = 0;
        nextchar = ' ';
        break;
    case '\n':  
        ++line;
        column = 0;
        break;
    case '\t':
        ++column;
        nextchar = ' ';
        break;
    default:                                    // every other character counts as one
        ++column;
        break;
    }
}

//
//      readchar - get the next character from the input stream
//

void PPStream::readchar()
{
    again:
    getnextchar();

    if (nextchar < 0) {
        nextchar = -1;
        return;                                 // end of file
    }

    if (!iscomment) {
        switch (nextchar) {
        case '\f':
        case '\v':
        case '\t':
        case ' ':
            break;
#if 0
        case '/':                               // look for a comment
            commentstartline = nextl;
            commentstartcolumn = nextc;
            getnextchar();
            if (nextchar == '/') {
                // have a C++ comment

                do {
                    getnextchar();
                    commentendline = nextl;
                    commentendcolumn = nextc;
                }
                while (nextchar != '\n' && nextchar != -1);

                iscomment = true;
                nextchar = '\n';
                break;
            }

            if (nextchar == '*') {
                // have a C comment

                commentloop:
                do
                    getnextchar();
                while (nextchar != '*' && nextchar != -1);

                // skip any number of asterisks
                while (nextchar == '*')
                    getnextchar();

                if (nextchar != -1) {
                    if (nextchar != '/')
                        goto commentloop;
                }

                commentendline = nextl;
                commentendcolumn = nextc + 1;
                iscomment = true;
                if (nextchar == -1) {
                    psp.error(pw::Error::ERROR,
                              commentstartline, commentstartcolumn, 0, 0,
                              "Unterminated comment.");
                    return;
                } else
                    nextchar = ' ';             // comment == space
                break;
            }

            first = new Stream(first, "", "", nextchar, false);
            nextchar = '/';
            break;
#endif

        case '\\':                              // look for an escaped newline
            getnextchar();
            if (nextchar == '\n')
                goto again;
            first = new Stream(first, "", "", nextchar, false);
            nextchar = '\\';
            break;

        case '?':                               // start of a trigraph?
            getnextchar();
            if (options->trigraphs && nextchar == '?') {
                // may be a trigraph

                getnextchar();
                switch (nextchar) {
                case '=':
                    nextchar = '#';
                    break;
                case '(':
                    nextchar = '[';
                    break;
                case '/':
                    nextchar = '\\';
                    break;
                case ')':
                    nextchar = ']';
                    break;
                case '\'':
                    nextchar = '^';
                    break;
                case '<':
                    nextchar = '{';
                    break;
                case '!':
                    nextchar = '|';
                    break;
                case '>':
                    nextchar = '}';
                    break;
                case '-':
                    nextchar = '~';
                    break;
                default:
                    first = new Stream(first, "", "?", nextchar, false);
                    nextchar = '?';
                    break;
                }
            } else {
                // not a trigraph

                first = new Stream(first, "", "", nextchar, false);
                nextchar = '?';
            }
            break;
        }

    }

    if (nextchar == ' ') {
        if (hasspace)
            goto again;                         // only one space
        hasspace = true;
    } else
        hasspace = false;
}

//
//      escape - gather an escape sequence
//
void PPStream::escape(std::string& string)
{
    string += '\\';
    readchar();
    switch (nextchar) {
    case '\'':
    case '"':
        // fall through
    case '?':
    case '\\':
    case 'a':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
    case 'v':
        string += nextchar;
        readchar();
        break;
    case 'x':                                   // hex escape
        string += nextchar;
        readchar();
        if (!isxdigit(nextchar))
            psp.error(pw::Error::ERROR,
                      startline, startcolumn, endline, endcolumn,
                      "\"\\x\" with no trailing hexadecimal digits.");
        while (isxdigit(nextchar)) {
            string += nextchar;
            readchar();
        }
        break;

    case '0':
    case '1':
    case '2':
    case '3':                                   // octal escape
    case '4':
    case '5':
    case '6':
    case '7':
        string += nextchar;
        readchar();
        if (nextchar >= '0' && nextchar <= '7') {
            string += nextchar;
            readchar();
        }
        break;

    default:
        psp.error(pw::Error::UNDEFINED,
                  startline, startcolumn, endline, endcolumn,
                  "Undefined character \"%c\" in escape sequence.", nextchar);
        string += nextchar;
        readchar();
    }
}

};
