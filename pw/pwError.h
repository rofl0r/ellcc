/*
 *    pwError.h - Header file for the error object.
 *
 *    Copyright (C) 2008, Richard Pennington.
 */

#ifndef pwError_h
#define pwError_h

#include <stdarg.h>
#include <string>

namespace pw {

class Error {
public:
    friend class ErrorList;
    enum Type {
        // Listed in order of least to most severe for sorting.
        STYLE, INFO, UNDEFINED, WARNING, ERROR, INTERNAL, FATAL,
        ERRORCNT
    };

    bool isError()                              // Is this an error or a warning?
        { return isError(type); }
    static bool isError(Type type)              // Is this an error or a warning?
        { return type == ERROR || type == FATAL || type == INTERNAL; }

    enum Info {
        MORE, STANDARD,
        INFOCNT
    };

    bool info(Info which, const char* format, ...);
    static bool info(Error* ep, Info which, const char* format, ...);
    bool info(Error::Info which, const char* format, va_list ap);
    const char* modifier()                      // Return a string representing the error modifier.
        { return modifier(type); }
    static const char* modifier(Type type);

private:
    static int compare(const void* a, const void* b);
    std::string string;                               // String associated with error.
    std::string file;                              // File associated with error.
    int startline, startcolumn;                 // Error position in source.
    int endline, endcolumn;                     // If startline or endline == 0, not available.
    Type type;                                  // Type of error.
    int count[INFOCNT];                         // Count of info messages.
    const char** infoMsgs[INFOCNT];             // Info messages.
};

class ErrorList {
public:
    ErrorList();
    ~ErrorList();
    Error* add(Error::Type type,
                 const std::string& file, int startline, int startcolumn, int endline, int endcolumn,
                 const char *format, va_list ap);
    void position(std::string& buffer,
                  const std::string& file, int startline, int startcolumn, int endline, int endcolumn,
                  bool trailer);
    void output(FILE* fp, Error* ep);
    void print(FILE* fp);
    void sort();
    enum Mode {
        NOFLAGS      = 0,
        LINEONLY     = 0x00000001,              // Show the error line number only.
        STARTONLY    = 0x00000002,              // Show the starting point of the error only.
        ALLMODES     = LINEONLY|STARTONLY,
        STDIO        = 0x00000004               // Input is from stdin, output goes to stdout immediately.
    };
    void setMode(int mode)                      // Set mode flags.
        { flags |= mode; }
    void clearMode(int mode)                    // Clear mode flags.
        { flags &= ~mode; }
private:
    int count;                                  // Number of errors in the list.
    Error** messages;                         // Error messages.
    int flags;                                  // Error flags.
};

};

#endif
