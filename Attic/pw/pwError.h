/** @file
 * Error message handling.
 * @author Richard Pennington
 * @date June 30, 2008
 *
 * Copyright (C) 2008, Richard Pennington.
 */

#ifndef pwError_h
#define pwError_h

#include <stdarg.h>
#include <setjmp.h>
#include <string>
#include "pwArray.h"

namespace pw {

/** An instance of an error.
 */
class Error {
public:
    friend class ErrorList;
    /** Types of errors.
     */
    enum Type {
        // Listed in order of least to most severe for sorting.
        STYLE,                                  ///< A style message.
        INFO,                                   ///< An informational message.
        UNDEFINED,                              ///< An undefined behavior message.
        WARNING,                                ///< A warning.
        ERROR,                                  ///< An error.
        INTERNAL,                               ///< An internal consistancy check failed.
        FATAL,                                  ///< A fatal error has occurred.

        ERRORCNT                                ///< The number of error types.
    };

    /** Classify an error.
     * @return true if the error #Type is #ERROR or higher.
     */
    bool isError()                              // Is this an error or a warning?
        { return isError(type); }
    /** Classify an error.
     * @param type The type of the error.
     * @return true if the error #Type is #ERROR or higher.
     */
    static bool isError(Type type)              // Is this an error or a warning?
        { return type == ERROR || type == FATAL || type == INTERNAL; }

    /** Types of additional error information.
     */
    enum Info {
        MORE,                                   ///< More information on the message.
        STANDARD,                               ///< Information about standard compliance.

        INFOCNT                                 ///< The number of types.
    };

    /** Add information to an error.
     * @param which The type of information being added.
     * @param format The message string.
     * @param ... Additional arguments for the message.
     * @return true if the information has been added.
     */
    bool info(Info which, const char* format, ...);
    /** Add information to an error.
     * @param ep The error to which information should be added.
     * @param which The type of information being added.
     * @param format The message string.
     * @param ... Additional arguments for the message.
     * @return true if the information has been added.
     */
    static bool info(Error* ep, Info which, const char* format, ...);
    /** Add information to an error.
     * @param which The type of information being added.
     * @param format The message string.
     * @param ap Additional arguments for the message.
     * @return true if the information has been added.
     */
    bool info(Error::Info which, const char* format, va_list ap);
    /** Get a string representing the error modifier.
     * @return The string.
     */
    const char* modifier()
        { return modifier(type); }
    /** Get a string representing the error modifier.
     * @param type The error type.
     * @return The string.
     */
    static const char* modifier(Type type);

private:
     /** Compare two errors for sorting.
      * @param a The left error.
      * @param b The right error.
      * @return <0, 0 or >0 depending on the sort order.
      */
    static int compare(const void* a, const void* b);
    std::string string;                         ///< The string associated with error.
    const char* file;                           ///< The file associated with error.
    int startline;                              ///< The starting line of the error in the source file.
    int startcolumn;                            ///< The starting column of the error in the source file.
    int endline;                                ///< The ending line of the error in the source file.
    int endcolumn;                              ///< The ending column of the error in the source file.
    // If startline or endline == 0, that source position is not available.
    Type type;                                  ///< Type of error.
    pw::array<std::string> infoMsgs[INFOCNT];   ///< Info messages.
};

/** A list of errors.
 */
class ErrorList {
public:
    /** The constructor.
     */
    ErrorList();
    /** The destructor.
     */
    ~ErrorList();
    /** Add an error to the list.
     * @param type The error type.
     * @param startline The starting line of the error in the source.
     * @param startcolumn The starting column of the error in the source.
     * @param endline The ending line of the error in the source.
     * @param endcolumn The ending column of the error in the source.
     * @param format The message string.
     * @param ap Additional arguments for the message.
     */
    Error* vadd(Error::Type type,
                 int startline, int startcolumn, int endline, int endcolumn,
                 const char *format, va_list ap);
    /** Add an error to the list.
     * @param type The error type.
     * @param sl The starting line of the error in the source.
     * @param sc The starting column of the error in the source.
     * @param el The ending line of the error in the source.
     * @param ec The ending column of the error in the source.
     * @param string The message string.
     * @param ... Additional arguments for the message.
     */
    Error* add(Error::Type type, int sl, int sc, int el, int ec, const char* string, ...);
    /** Add an error to the list.
     * @param type The error type.
     * @param file The file containing the error.
     * @param startline The starting line of the error in the source.
     * @param startcolumn The starting column of the error in the source.
     * @param endline The ending line of the error in the source.
     * @param endcolumn The ending column of the error in the source.
     * @param format The message string.
     * @param ap Additional arguments for the message.
     */
    Error* vadd(Error::Type type, const char* file,
                 int startline, int startcolumn, int endline, int endcolumn,
                 const char *format, va_list ap);
    /** Add an error to the list.
     * @param type The error type.
     * @param file The file containing the error.
     * @param sl The starting line of the error in the source.
     * @param sc The starting column of the error in the source.
     * @param el The ending line of the error in the source.
     * @param ec The ending column of the error in the source.
     * @param string The message string.
     * @param ... Additional arguments for the message.
     */
    Error* add(Error::Type type, const char* file, int sl, int sc, int el, int ec, const char* string, ...);
    /** Get an error position in a standard format.
     * @param[out] buffer The position as a string.
     * @param file The source file name.
     * @param startline The starting line of the error in the source.
     * @param startcolumn The starting column of the error in the source.
     * @param endline The ending line of the error in the source.
     * @param endcolumn The ending column of the error in the source.
     * @param trailer true if a colon (':') should be output after the position.
     */
    void position(std::string& buffer,
                  const char* file, int startline, int startcolumn, int endline, int endcolumn,
                  bool trailer);
    /** Output an error message.
     * @param fp The file to use.
     * @param ep The error to output.
     */
    void output(FILE* fp, Error* ep);
    /** Send an error list to a file.
     * @param fp The file to use.
     */
    void print(FILE* fp);
    void sort();                                ///< Sort an error list.
    /** Flags modifying error list behavior.
     */
    enum Mode {
        NOFLAGS      = 0,
        LINEONLY     = 0x00000001,              ///< Show the error line number only.
        STARTONLY    = 0x00000002,              ///< Show the starting point of the error only.
        ALLMODES     = LINEONLY|STARTONLY,      ///< All line number modes.
        STDIO        = 0x00000004               ///< Input is from stdin, output goes to stdout immediately.
    };
    /** Set mode flags.
     * @param mode The flags to set.
     */
    void setMode(int mode)
        { flags |= mode; }
    /** Clear mode flags.
     * @param mode The flags to clear.
     */
    void clearMode(int mode)
        { flags &= ~mode; }
    /** Check for errors.
     * @return true if errors have been encountered.
     */
    bool hasErrors() { return haveErrors; }
    /** Get the number of errors for a type.
     * @param type The error type.
     * @return The number of messages of that type.
     */
    int errorCount(int type) { return errorcount[type]; }
    const char* file;                           ///< The current file being processed.
    /** Set a target for fatal errors.
     * @param buf The target.
     */
    void setFatal(jmp_buf* buf) { fatal = buf; }
    bool recentErrors;                          ///< true if errors have occured since this was cleared.
private:
    int count;                                  ///< Number of errors in the list.
    Error** messages;                           ///< Error messages.
    int flags;                                  ///< Error flags.
    bool haveErrors;                            ///< true if some errors have occurred.
    int errorcount[Error::ERRORCNT];            ///< Number of errors encountered by type.
    jmp_buf* fatal;				///< Fatal exit.
};

};

#endif
