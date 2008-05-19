//
//	EllError.c - Standard error handling.
//

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "EllError.h"

//
// EllErrorList - Error list constructor.
//
EllErrorList::EllErrorList()
{
    count = 0;
    messages = NULL;
    flags = 0;
}

//
// ~EllErrorList - destruct an error list.
//
EllErrorList::~EllErrorList()
{
    for (int i = 0; i < count; ++i) {
        delete messages[i]->string;

        for (int j = 0; j < EllError::INFOCNT; ++j) {
            if (messages[i]->infoMsgs[j]) {
                for (int k = 0; k < messages[i]->count[j]; ++k)
                    delete messages[i]->infoMsgs[j][k];
                delete messages[i]->infoMsgs[j];

            }
        }
        delete messages[i];
    }

    delete messages;
}


//
// add - Add an error to an error list.
//
EllError *EllErrorList::add(EllError::Type type, const std::string& file,
                          int startline, int startcolumn, int endline, int endcolumn,
                          const char *format, va_list ap)
{
    EllError *ep = new EllError;
    EllError **epp;
    int i;
    char buffer[1024];	// RICH

    for (i = 0; i < EllError::INFOCNT; ++i) {
        ep->count[i] = 0;
        ep->infoMsgs[i] = NULL;
    }
    ep->type = type;
    vsprintf(buffer, format, ap);
    ep->string = buffer;
    if (ep->string == NULL) {
        free(ep);
        return NULL;
    }
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

    epp = (EllError **)realloc(messages, (count + 1) * sizeof(EllError *));
    if (epp == NULL) {
        delete ep->string;
        delete ep;
        return NULL;
    }

    epp[count] = ep;
    messages = epp;
    ++count;
    return ep;
}

//
// info - Add information to an error message.
//
bool EllError::info(EllError *errormsg, EllError::Info which, const char *format, ...)
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
bool EllError::info(EllError::Info which, const char *format, ...)
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
bool EllError::info(EllError::Info which, const char *format, va_list ap)
{
    const char **ip;
    char buffer[1024];	// RICH

    if (which >= EllError::INFOCNT) {
        return false;
    }

    ip = (const char **)realloc(infoMsgs[which], (count[which] + 1) * sizeof( char *));
    if (ip == NULL) {
        return false;
    }

    infoMsgs[which] = ip;
    vsprintf(buffer, format, ap);
    infoMsgs[which][count[which]] = buffer;
    if (infoMsgs[which][count[which]] == NULL) {
        return false;
    }

    ++count[which];
    return true;
}

//
// compare - compare two error messages
//
int
EllError::compare(const void *a, const void *b)
{
    const EllError *first = *(const EllError **)a;
    const EllError *second = *(const EllError **)b;
    int i;

    // Sort by file name.
    if (first->file.length() && second->file.length()) {
        i = first->file.compare(second->file);
    } else if (first->file.length() == 0 && second->file.length() == 0) {
        i = 0;                                  // Both empty.
    } else if (first->file.length()) {
        return 1;                               // First has a name.
    } else {
        return -1;                              // Second has a name.
    }

    if (i != 0) {
        return i;                               // Alphabetically by file name.
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
void EllErrorList::sort()
{
    qsort(messages, count, sizeof(EllError *), EllError::compare);
}


//
// errorPosition - Format an error's position in a standard way.
//
void errorPosition(EllErrorList *errorlist, std::string& buffer, const std::string& file,
              int startline, int startcolumn, int endline, int endcolumn,
              bool trailer)
{
    errorlist->position(buffer, file, startline, startcolumn, endline, endcolumn, trailer);
}

//
// position - Format an error's position in a standard way.
//
void EllErrorList::position(std::string& buffer, const std::string& file,
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
const char *EllError::modifier(EllError::Type type) {
    switch (type) {
    case EllError::WARNING:
        return "warning";
    case EllError::UNDEFINED:
        return "undefined";
    case EllError::INFO:
        return "information";
    case EllError::FATAL:
        return "fatal error";
    case EllError::INTERNAL:
        return "internal error";
    case EllError::ERROR:
        return "error";
    case EllError::STYLE:
        return "style";
    default:
        return "unknown";
    }
}

//
// output - Output an error message.
//
void EllErrorList::output(FILE* fp, EllError* ep)
{
    const char *modifier;
    std::string buffer;

    modifier = ep->modifier();
    position(buffer, ep->file, ep->startline, ep->startcolumn,
             ep->endline, ep->endcolumn, true);

    fprintf(fp, "@s %s - %s\n", &buffer, modifier, ep->string);

    // Print error information, if any.
    for (int which = 0; which < EllError::INFOCNT; ++which) {
        for (int j = 0; j < ep->count[which]; ++j) {
            fprintf(fp, "%*s%s\n", (which+1)*4, " ", 
                   ep->infoMsgs[which][j]);
        }
    }
}

//
// print - Print error messages in the standard format.
//
void EllErrorList::print(FILE *fp)
{
    for (int i = 0; i < count; ++i) {
        output(fp, messages[i]);
    }
}
