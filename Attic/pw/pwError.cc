/** @file
 * Standard error message handling.
 * @author Richard Pennington
 * @date July 1, 2008
 *
 * Copyright (C) 2008, Richard Pennington.
 */

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "pwError.h"

namespace pw {

//
// ErrorList - Error list constructor.
//
ErrorList::ErrorList()
{
    count = 0;
    messages = NULL;
    flags = 0;
    haveErrors = false;
    recentErrors = false;
    for (int i = 0; i < Error::ERRORCNT; ++i) {
        errorcount[i] = 0;
    }
    fatal = NULL;
}

//
// ~ErrorList - destruct an error list.
//
ErrorList::~ErrorList()
{
    for (int i = 0; i < count; ++i) {
        delete messages[i];
    }

    delete messages;
}


Error* ErrorList::add(Error::Type type, const char* file, int sl, int sc, int el, int ec, const char* string, ...)
{
    va_list ap;
    Error *ep;
    va_start(ap, string);
    ep = vadd(type, file, sl, sc, el, ec, string, ap);
    va_end(ap);
    return ep;
}

Error* ErrorList::add(Error::Type type, int sl, int sc, int el, int ec, const char* string, ...)
{
    va_list ap;
    Error *ep;
    va_start(ap, string);
    ep = vadd(type, sl, sc, el, ec, string, ap);
    va_end(ap);
    return ep;
}

//
// add - Add an error to an error list.
//
Error *ErrorList::vadd(Error::Type type, const char* file,
                          int startline, int startcolumn, int endline, int endcolumn,
                          const char *format, va_list ap)
{
    Error *ep = new Error;
    Error **epp;
    char buffer[1024];	// RICH

    ep->type = type;
    vsprintf(buffer, format, ap);
    ep->string = buffer;
    ep->file = file;
    ep->startline = startline;
    ep->startcolumn = startcolumn;
    ep->endline = endline;
    ep->endcolumn = endcolumn;
    if (flags & STDIO) {
        // Send the error immediately.
        output(stdout, ep);
        delete ep;
        // RICH: No info can be added to this error.
        return NULL;
    }

    epp = (Error **)realloc(messages, (count + 1) * sizeof(Error *));
    if (epp == NULL) {
        delete ep;
        return NULL;
    }

    epp[count] = ep;
    messages = epp;
    ++count;
    ++errorcount[type];                         // Increment the error count for this type.
    if (ep->isError()) {
        haveErrors = true;                      // Inhibit subsequent processing.
        recentErrors = true;
    }
    if (type == Error::FATAL && fatal) {
        // A fatal error has occured.
        longjmp(*fatal, 1);
    }
    return ep;
}

//
// add - Add an error to an error list.
//
Error *ErrorList::vadd(Error::Type type,
                          int startline, int startcolumn, int endline, int endcolumn,
                          const char *format, va_list ap)
{
    return vadd(type, file, startline, startcolumn, endline, endcolumn, format, ap);
}

//
// info - Add information to an error message.
//
bool Error::info(Error *errormsg, Error::Info which, const char *format, ...)
{
    va_list ap;

    if (errormsg == NULL) {
        return false;
    }

    va_start(ap, format);
    bool status = errormsg->info(which, format, ap);
    va_end(ap);
    return status;
}

//
// info - Add information to an error message.
//
bool Error::info(Error::Info which, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    bool status = info(which, format, ap);
    va_end(ap);
    return status;
}

//
// info - Add information to an error message.
//
bool Error::info(Error::Info which, const char *format, va_list ap)
{
    char buffer[1024];	// RICH

    if (which >= Error::INFOCNT) {
        return false;
    }

    vsprintf(buffer, format, ap);
    infoMsgs[which][infoMsgs[which].size()] = buffer;
    return true;
}

//
// compare - compare two error messages
//
int
Error::compare(const void *a, const void *b)
{
    const Error *first = *(const Error **)a;
    const Error *second = *(const Error **)b;
    int i;

    // Sort by file name.
    if ((i = strcmp(first->file, second->file)) != 0) {
        return i;
    }

    // Sort by source position.
    if (first->startline < second->startline) {
        return -1;
    }
    if (first->startline > second->startline) {
        return 1;
    }
    if (first->endline < second->endline) {
        return -1;
    }
    if (first->endline > second->endline) {
        return 1;
    }
    if (first->startcolumn < second->startcolumn) {
        return -1;
    }
    if (first->startcolumn > second->startcolumn) {
        return 1;
    }
    if (first->endcolumn < second->endcolumn) {
        return -1;
    }
    if (first->endcolumn > second->endcolumn) {
        return 1;
    }

    // Sort by error severity.
    if (first->type > second->type) {
        return 1;
    }
    if (first->type < second->type) {
        return -1;
    }
    return 0;
}

//
// sort - Sort an error list.
//
void ErrorList::sort()
{
    qsort(messages, count, sizeof(Error *), Error::compare);
}


//
// errorPosition - Format an error's position in a standard way.
//
void errorPosition(ErrorList *errorlist, std::string& buffer, const char* file,
              int startline, int startcolumn, int endline, int endcolumn,
              bool trailer)
{
    errorlist->position(buffer, file, startline, startcolumn, endline, endcolumn, trailer);
}

//
// position - Format an error's position in a standard way.
//
void ErrorList::position(std::string& buffer, const char* file,
                           int startline, int startcolumn, int endline, int endcolumn,
                           bool trailer)
{
    buffer = file;

    if (startline) {
        // have a start line
        if (buffer.length()) {
            buffer += ':';
        }

        char temp[1024];	// RICH
        sprintf(temp, "%d", startline);
        buffer += temp;
        if (!(flags & LINEONLY) && startcolumn >= 0) {
            sprintf(temp, ".%d", startcolumn);
            buffer += temp;
        }

        if (!(flags & (LINEONLY|STARTONLY)) && endline) {
            // have an end line
            sprintf(temp, "-%d", endline);
            buffer += temp;
            if (endcolumn >= 0)
                sprintf(temp, ".%d", endcolumn);
                buffer += temp;
        }
    }

    if (trailer && buffer.length()) {
        buffer += ':';
    }
}

//
// modifier - Return a modifier string.
//
const char *Error::modifier(Error::Type type) {
    switch (type) {
    case Error::WARNING:
        return "warning";
    case Error::UNDEFINED:
        return "undefined";
    case Error::INFO:
        return "information";
    case Error::FATAL:
        return "fatal error";
    case Error::INTERNAL:
        return "internal error";
    case Error::ERROR:
        return "error";
    case Error::STYLE:
        return "style";
    default:
        return "unknown";
    }
}

//
// output - Output an error message.
//
void ErrorList::output(FILE* fp, Error* ep)
{
    const char *modifier;
    std::string buffer;

    modifier = ep->modifier();
    position(buffer, ep->file, ep->startline, ep->startcolumn,
             ep->endline, ep->endcolumn, true);

    fprintf(fp, "%s %s - %s\n", buffer.c_str(), modifier, ep->string.c_str());

    // Print error information, if any.
    for (int which = 0; which < Error::INFOCNT; ++which) {
        for (int j = 0; j < ep->infoMsgs[which].size(); ++j) {
            fprintf(fp, "%*s%s\n", (which+1)*4, " ", 
                   ep->infoMsgs[which][j].c_str());
        }
    }
}

//
// print - Print error messages in the standard format.
//
void ErrorList::print(FILE *fp)
{
    for (int i = 0; i < count; ++i) {
        output(fp, messages[i]);
    }
}

};
