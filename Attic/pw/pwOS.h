/** @file
 * A generic OS interface.
 * @author Richard Pennington
 * @date July 1, 2008
 *
 * Copyright (C) 2008, Richard Pennington.
 */

#ifndef pwOS_h
#define pwOS_h

#include <stdio.h>
#include <string>

namespace pw {

/** Find the name of the executable program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return The name of the executable.
 */
extern std::string findExecutable(int argc, char** argv);
/** Open a binary file.
 * @param name The file name.
 * @param mode The file mode.
 * @return A FILE pointer or NULL if the open fails.
 */
extern FILE* bfopen(const std::string& name, const char* mode);
/** Open a text file.
 * @param name The file name.
 * @param mode The file mode.
 * @return A FILE pointer or NULL if the open fails.
 */
extern FILE* tfopen(const std::string& name, const char* mode);
/** Reopen a binary file.
 * @param name The file name.
 * @param mode The file mode.
 * @param fp The currently open file.
 * @return A FILE pointer or NULL if the open fails.
 */
extern FILE* bfreopen(const std::string& name, const char* mode, FILE* fp);
/** Reopen a text file.
 * @param name The file name.
 * @param mode The file mode.
 * @param fp The currently open file.
 * @return A FILE pointer or NULL if the open fails.
 */
extern FILE* tfreopen(const std::string& name, const char* mode, FILE* fp);
/** Close a file.
 * @param fp The file pointer.
 * @return < 0 if the close fails.
 */
extern int fclose(FILE* fp);
/** Flush a file.
 * @param fp The file pointer.
 * @return < 0 if the flush fails.
 */
extern int fflush(FILE* fp);
/** Parse a file name.
 * @param filename The name to parse.
 * @param[out] path The parsed path.
 * @param[out] name The parsed base name.
 * @param[out] extension The parsed extension.
 */
extern void parseFilename(const std::string& filename, std::string& path, std::string& name, std::string& extension);
/** Build a file name.
 * @param path The file path.
 * @param name The file base name.
 * @param extension The file extension.
 * @return The full file name.
 */
extern std::string buildFilename(const std::string& path, const std::string& name, const std::string& extension = "");
/** Check for a fully qualifyed file name.
 * @param path The file name.
 * @return true if the name is a full path name.
 */
extern bool fullPath(const std::string& path);

};

#endif
