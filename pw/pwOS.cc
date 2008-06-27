/*
 *    pwOS.c - The generic OS interface.
 *
 *    Copyright (C) 2004, Richard Pennington.
 */

#include <ctype.h>
#include <stdlib.h>
#include "pwOS.h"

namespace pw {

#if defined(__windows__)
#include <process.h>
#include <io.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#endif

#define STRINGMAX 32767

#if RICH        // Not for now.
//
// findLibrary - Find a loadable library and return its handle.
//
void* findLibrary(const std::string& directory, const std::string& name)
{
    std::string libname;
    void *handle = NULL;
    libname = directory;

#if defined(__windows__)
    libname += name;
    libname += ".dll";
    libname = buildFilename(directory, name, ".dll");
    handle = LoadLibrary(libname.c_str());
#else
    libname = "lib";
    libname += name;
    libname = buildFilename(directory, libname, ".so");
    handle = dlopen(libname.c_str(), RTLD_LAZY);
#endif

    return handle;
}

//
// findLibrarySymbol - Find a symbol in a loadable library.
//
void* findLibrarySymbol(void *handle, const std::string& symbol)
{

    if (!handle) {
        return NULL;
    }

    void *result;

#if defined(__windows__)
    result =  (void *)GetProcAddress((HINSTANCE)handle, symbol.c_str());
#else
    result = dlsym(handle, symbol.c_str());
#endif
    return result;
}
#endif

//
// findExecutable - Find the name of the executable program.
//
std::string findExecutable(int argc, char **argv)
{
    static std::string name;
    const char *p;

    char buffer[FILENAME_MAX + 1];

    if (name.length()) {
        // The name has already been found.
        return name;
    }

#if defined(__windows__)
    GetModuleFileName(NULL, buffer, sizeof(buffer));
    for (p = buffer; *p != '\0'; p++) {
        if (*p == '\\') {
            *p = '/';
        }
    }

    name = buffer;
#else
    struct stat statBuf;
    int len;
    std::string temp, nameString;

    name = argv[0];                             // Copy argv[0].
    for (p = name.c_str(); *p != '\0'; p++) {
        if (*p == '/') {
            /*
             * The name contains a slash, so use the name directly
             * without doing a path search.
             */

            goto gotName;
        }
    }

    p = getenv("PATH");
    if (p == NULL) {
        /*
         * There's no PATH environment variable; use the default that
         * is used by sh.
         */

        p = ":/bin:/usr/bin";
    } else if (*p == '\0') {
        /*
         * An empty path is equivalent to ".".
         */

        p = "./";
    }

    /*
     * Search through all the directories named in the PATH variable
     * to see if argv[0] is in one of them.  If so, use that file
     * name.
     */

    for (;;) {
        while (isspace(*p)) {
            p++;
        }
        temp = "";
        while ((*p != ':') && (*p != 0)) {
            temp += *p;
            p++;
        }
        if (p != name) {
            len = temp.length();

            if (len && temp[len - 1] != '/') {
                temp += "/";
            }
        }

        temp += argv[0];
        name = temp;

        if ((access(name.c_str(), X_OK) == 0)
            && (stat(name.c_str(), &statBuf) == 0)
            && S_ISREG(statBuf.st_mode)) {
            goto gotName;
        }
        if (*p == '\0') {
            break;
        } else if (*(p+1) == 0) {
            p = "./";
        } else {
            p++;
        }
    }
    goto done;

    /* If the name starts with "/" then just copy it.
     */

gotName:
    if (name[0] == '/') {
        goto done;
    }

    /*
     * The name is relative to the current working directory.  First
     * strip off a leading "./", if any, then add the full path name of
     * the current working directory.
     */

    if ((name[0] == '.') && (name[1] == '/')) {
        name = name.substr(2);
    }

    if (getcwd(buffer, FILENAME_MAX + 1) == NULL) {
        buffer[0] = '\0';
    }

    temp = buffer;
    len = temp.length();

    if (len && temp[len - 1] != '/') {
        temp += "/";
    }

    name = temp + name;

done:
#endif

    argv[0] = (char*)name.c_str();                // Save a copy in argv[0].    
    return name;
}


//
// bfopen - Open a file for binary buffered access.
//
FILE* bfopen(const std::string& name, const char* mode)
{
    FILE* fp;

#if defined(__windows__)
    char buf[5];

    strncpy(buf, mode, 3);
    strcat(buf, "b");
    fp = ::fopen(name.c_str(), buf);
#else
    fp = ::fopen(name.c_str(), mode);
#endif

    return fp;
}

//
// tfopen - Open a file for text buffered access.
//
FILE* tfopen(const std::string& name, const char* mode)
{
    FILE* fp;

#if defined(__windows__)
    char buf[5];

    strncpy(buf, mode, 3);
    strcat(buf, "t");
    fp = ::fopen(name.c_str(), buf);
#else
    fp = ::fopen(name.c_str(), mode);
#endif

    return fp;
}

//
// bfreopen - Reopen a file for binary buffered access.
//
FILE* bfreopen(const std::string& name, const char* mode, FILE* fp)
{
#if defined(__windows__)
    char buf[5];

    strncpy(buf, mode, 3);
    strcat(buf, "b");
    fp = ::freopen(name.c_str(), buf, fp);
#else
    fp = ::freopen(name.c_str(), mode, fp);
#endif

    return fp;
}

//
// tfreopen - Reopen a file for text buffered access.
//
FILE* tfreopen(const std::string& name, const char* mode, FILE* fp)
{
#if defined(__windows__)
    char buf[5];

    strncpy(buf, mode, 3);
    strcat(buf, "t");
    fp = ::freopen(name.c_str(), buf, fp);
#else
    fp = ::freopen(name.c_str(), mode, fp);
#endif

    return fp;
}

//
// fclose - Close a file.
//
int fclose(FILE* fp)
{
    return ::fclose(fp);
}

//
// fflush - Flush a file.
//
int fflush(FILE* fp)
{
    return ::fflush(fp);
}

#if defined(__windows__)
static char DIRTERM[] = "/\\";
#define BASETERM '.'
#define PATHSEP '/'
#else
static const char DIRTERM[] = "/";              // The set of characters that terminate a directory name.
#define BASETERM '.'		                // The character that terminates a base name.
#define PATHSEP '/'		                // Path component separator.
#endif

//
// parseFilename - Parse a filename in "filename" and return the component information in
// appropriate strings.
//
void parseFilename(const std::string& filename, std::string& path, std::string& name, std::string& extension)
{
    int index, ext;

    // Start with empty return values.
    path = "";
    name = "";
    extension = "";

    if (filename.length() == 0) {
        // Nothing to do.
        return;
    }

    // Find the directory name by finding the last occurrance of a character in DIRTERM.
    for (index = filename.length() - 1; index >= 0 ; --index) {
        for (const char *p = DIRTERM; *p; ++p) {
            if (*p == filename[index]) {
                // We have a terminator, break out of the loop.
                goto breakout;
            }
        }
    }
    
breakout:

    if (index >= 0) {
        // We found a path.
        path = filename.substr(0, index);
        name = filename.substr(index + 1);
    } else {
        name = filename;
    }

    ++index;    // Index contains the start of the name component.
    
    // Find the extension by finding the last occurrance of BASETERM.
    for (ext = name.length() - 1; ext >= 0 ; --ext) {
        if (filename[ext] == BASETERM) {
            // We found an extension separator.
            break;
        }
    }
    
    if (ext >= 0) {
        // We have an extension.
        extension = name.substr(ext + 1);
        name = name.substr(index, ext);
    }
}

//
// buildFilename - Build a complete filename out of the components and return a pointer to it.
//
std::string buildFilename(const std::string& path, const std::string& name, const std::string& extension)
{
    std::string result;

    // Add the directory name if one exists.
    if (path.length()) {
        const char *p;

        // A path exists, check for a terminating separator.

        result = path;
        for (p = DIRTERM; *p; ++p) {
            if (result[result.length() - 1] == *p) {
                break;
            }
        }

        if (*p == '\0') {
            // Separator not found.
            result += DIRTERM[0];
        }
    }
    

    // Add the base name.
    result += name;

    // Add the extension.
    if (extension.length()) {
        // Terminate the basename component if an extension exists.
        if (result[result.length() - 1] != BASETERM && extension[0] != BASETERM) {
            result += BASETERM;
        }

        result += extension;
    }

    return result;
}

/* Check if a path is completely specified.
 */
bool fullPath(const std::string& path)
{
    return path[0] == '/';
}

};

