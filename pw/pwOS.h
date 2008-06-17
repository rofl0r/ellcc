/*
 *    pwOS.h - Header file for the generic OS interface.
 *
 *    Copyright (C) 2008, Richard Pennington.
 */

#ifndef pwOS_h
#define pwOS_h

#include <stdio.h>
#include <string>

namespace pw {

extern void* findLibrary(const std::string& directory, const std::string& name);    // Find a dynamic library.
extern void* findLibrarySymbol(void* handle, const std::string& symbol);         // Find a symbol in a library.
extern std::string FindExecutable(int argc, char** argv);        // Find the executable file name.

extern FILE* bfopen(const std::string& name, const char* mode);  // Open a binary file.
extern FILE* tfopen(const std::string& name, const char* mode);  // Open a text file.
extern FILE* bfreopen(const std::string& name, const char* mode, FILE* fp);  // Reopen a binary file.
extern FILE* tfreopen(const std::string& name, const char* mode, FILE* fp);  // Reopen a text file.
extern int fclose(FILE* fp);                                  // Close a file.
extern int fflush(FILE* fp);                                  // Flush a file.

extern void parseFilename(const std::string& filename, std::string& path, std::string& name, std::string& extension);
extern std::string buildFilename(const std::string& path, const std::string& name, const std::string& extension = "");
extern bool fullPath(const std::string& path);

};

#endif
