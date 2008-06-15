/*
 *    pwOS.c - The generic OS interface.
 *
 *    Copyright (C) 2004, Richard Pennington.
 */

#include <ctype.h>
#include <stdlib.h>
#include "pwOS.h"

namespace pw {

#if OSWIN32
#include <process.h>
#include <io.h>
#include <windows.h>
#endif

#if defined(__linux__)
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#endif

#define STRINGMAX 32767

//
// findLibrary - Find a loadable library and return its handle.
//
void* findLibrary(const std::string& directory, const std::string& name)
{
    std::string libname;
    void *handle = NULL;
    const char *p;

    libname = directory;

#if defined(__linux__)
    libname = "lib";
    libname += name;
    libname = buildFilename(directory, libname, ".so");
    p = libname.c_str();
    handle = dlopen(p, RTLD_LAZY);
#elif defined(__windows__)
    libname += name;
    libname += ".dll";
    libname = buildFilename(directory, name, ".dll");
    p = libname.c_str();
    handle = LoadLibrary(p);
#else
#error No findLibrary body defined.
#endif

    delete p;
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
    const char *p = symbol.c_str();

#if defined(__linux__)
    result = dlsym(handle, p);
#elif defined(__windows__)
    result =  (void *)GetProcAddress((HINSTANCE)handle, p);
#else
#error No findLibrarySymbol body defined.
#endif
    delete p;
    return result;
}

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
#elif defined(__linux__)
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
#else
#error No findExecutable body defined.
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
    const char* p = name.c_str();

#if OSWIN32
    char buf[5];

    strncpy(buf, mode, 3);
    strcat(buf, "b");
    fp = ::fopen(p, buf);
#else
    fp = ::fopen(p, mode);
#endif

    delete p;
    return fp;
}

//
// tfopen - Open a file for text buffered access.
//
FILE* tfopen(const std::string& name, const char* mode)
{
    FILE* fp;
    const char* p = name.c_str();

#if OSWIN32
    char buf[5];

    strncpy(buf, mode, 3);
    strcat(buf, "t");
    fp = ::fopen(p, buf);
#else
    fp = ::fopen(p, mode);
#endif

    delete p;
    return fp;
}

//
// bfreopen - Reopen a file for binary buffered access.
//
FILE* bfreopen(const std::string& name, const char* mode, FILE* fp)
{
    const char *p = name.c_str();

#if OSWIN32
    char buf[5];

    strncpy(buf, mode, 3);
    strcat(buf, "b");
    fp = ::freopen(p, buf, fp);
#else
    fp = ::freopen(p, mode, fp);
#endif

    delete p;
    return fp;
}

//
// tfreopen - Reopen a file for text buffered access.
//
FILE* tfreopen(const std::string& name, const char* mode, FILE* fp)
{
    const char* p = name.c_str();

#if OSWIN32
    char buf[5];

    strncpy(buf, mode, 3);
    strcat(buf, "t");
    fp = ::freopen(p, buf, fp);
#else
    fp = ::freopen(p, mode, fp);
#endif

    delete p;
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

#if defined(__linux__)
static const char DIRTERM[] = "/";              // The set of characters that terminate a directory name.
#define BASETERM '.'		                // The character that terminates a base name.
#define PATHSEP '/'		                // Path component separator.
#endif

#if     OSWIN32
static char DIRTERM[] = "/\\";
#define BASETERM '.'
#define PATHSEP '/'
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
    }

    ++index;    // Index contains the start of the name component.
    
    // Find the extension by finding the last occurrance of BASETERM.
    for (ext = filename.length() - 1; ext >= index ; --ext) {
        if (filename[ext] == BASETERM) {
            // We found an extension separator.
            break;
        }
    }
    
    if (ext >= index) {
        // We have an extension.
        name = filename.substr(index, ext);
        extension = filename.substr(ext + 1, filename.length());
    } else {
        // We have a name only, no extension.
        name = filename.substr(index, filename.length());
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

#ifndef cexecvp

#if OSWIN32

#define APPL_NONE 0
#define APPL_DOS 1
#define APPL_WIN3X 2
#define APPL_WIN32 3

/*
 *--------------------------------------------------------------------
 *
 * ApplicationType --
 *
 *	Search for the specified program and identify if it refers to a DOS,
 *	Windows 3.X, or Win32 program.  Used to determine how to invoke 
 *	a program, or if it can even be invoked.
 *
 *	It is possible to almost positively identify DOS and Windows 
 *	applications that contain the appropriate magic numbers.  However, 
 *	DOS .com files do not seem to contain a magic number; if the program 
 *	name ends with .com and could not be identified as a Windows .com
 *	file, it will be assumed to be a DOS application, even if it was
 *	just random data.  If the program name does not end with .com, no 
 *	such assumption is made.
 *
 *	The Win32 procedure GetBinaryType incorrectly identifies any 
 *	junk file that ends with .exe as a dos executable and some 
 *	executables that don't end with .exe as not executable.  Plus it 
 *	doesn't exist under win95, so I won't feel bad about reimplementing
 *	functionality.
 *
 * Results:
 *	The return value is one of APPL_DOS, APPL_WIN3X, or APPL_WIN32
 *	if the filename referred to the corresponding application type.
 *	If the file name could not be found or did not refer to any known 
 *	application type, APPL_NONE is returned and an error message is 
 *	left in interp.  .bat files are identified as APPL_DOS.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
ApplicationType(const char *originalName, char *fullPath)
{
    int applType, i;
    HANDLE hFile;
    char *ext, *rest;
    char buf[2];
    DWORD read;
    IMAGE_DOS_HEADER header;
    static char extensions[][5] = {"", ".com", ".exe", ".bat"};

    /* Look for the program as an external program.  First try the name
     * as it is, then try adding .com, .exe, and .bat, in that order, to
     * the name, looking for an executable.
     *
     * Using the raw SearchPath() procedure doesn't do quite what is 
     * necessary.  If the name of the executable already contains a '.' 
     * character, it will not try appending the specified extension when
     * searching (in other words, SearchPath will not find the program 
     * "a.b.exe" if the arguments specified "a.b" and ".exe").   
     * So, first look for the file as it is named.  Then manually append 
     * the extensions, looking for a match.  
     */

    applType = APPL_NONE;
    for (i = 0; i < (int) (sizeof(extensions) / sizeof(extensions[0])); i++) {
        lstrcpyn(fullPath, originalName, MAX_PATH - 5);
        lstrcat(fullPath, extensions[i]);

        SearchPath(NULL, fullPath, NULL, MAX_PATH, fullPath, &rest);

        /*
         * Ignore matches on directories or data files, return if identified
         * a known type.
         */

        if (GetFileAttributes(fullPath) & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        ext = strrchr(fullPath, '.');
        if ((ext != NULL) && (strcmpi(ext, ".bat") == 0)) {
            applType = APPL_DOS;
            break;
        }

        hFile = CreateFile(fullPath, GENERIC_READ, FILE_SHARE_READ, NULL, 
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            continue;
        }

        header.e_magic = 0;
        ReadFile(hFile, (void *) &header, sizeof(header), &read, NULL);
        if (header.e_magic != IMAGE_DOS_SIGNATURE) {
            /* 
             * Doesn't have the magic number for relocatable executables.  If 
             * filename ends with .com, assume it's a DOS application anyhow.
             * Note that we didn't make this assumption at first, because some
             * supposed .com files are really 32-bit executables with all the
             * magic numbers and everything.  
             */

            CloseHandle(hFile);
            if ((ext != NULL) && (strcmpi(ext, ".com") == 0)) {
                applType = APPL_DOS;
                break;
            }
            continue;
        }
        if (header.e_lfarlc != sizeof(header)) {
            /* 
             * All Windows 3.X and Win32 and some DOS programs have this value
             * set here.  If it doesn't, assume that since it already had the 
             * other magic number it was a DOS application.
             */

            CloseHandle(hFile);
            applType = APPL_DOS;
            break;
        }

        /* 
         * The DWORD at header.e_lfanew points to yet another magic number.
         */

        buf[0] = '\0';
        SetFilePointer(hFile, header.e_lfanew, NULL, FILE_BEGIN);
        ReadFile(hFile, (void *) buf, 2, &read, NULL);
        CloseHandle(hFile);

        if ((buf[0] == 'L') && (buf[1] == 'E')) {
            applType = APPL_DOS;
        } else if ((buf[0] == 'N') && (buf[1] == 'E')) {
            applType = APPL_WIN3X;
        } else if ((buf[0] == 'P') && (buf[1] == 'E')) {
            applType = APPL_WIN32;
        } else {
            continue;
        }
        break;
    }

    return applType;
}

/*    
 *----------------------------------------------------------------------
 *
 * BuildCommandLine --
 *
 *	The command line arguments are stored in linePtr separated
 *	by spaces, in a form that CreateProcess() understands.  Special 
 *	characters in individual arguments from argv[] must be quoted 
 *	when being stored in cmdLine.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static void
BuildCommandLine(int argc, const char **argv, char *linePtr)
{
    const char *start, *special;
    int quote, i;

    for (i = 0; i < argc; i++) {
        if (i > 0) {
            *linePtr++ = ' ';
        }

        quote = 0;
        for (start = argv[i]; *start != '\0'; start++) {
            if (isspace(*start)) {
                quote = 1;
                *linePtr++ = '"';
                break;
            }
        }

        start = argv[i];        
        for (special = argv[i]; ; ) {
            if ((*special == '\\') && 
                (special[1] == '\\' || special[1] == '"')) {
                strncpy(linePtr, start, special - start);
                linePtr += special - start;
                start = special;
                while (1) {
                    special++;
                    if (*special == '"') {
                        /* 
                         * N backslashes followed a quote -> insert 
                         * N * 2 + 1 backslashes then a quote.
                         */

                        strncpy(linePtr, start, special - start);
                        linePtr += special - start;
                        break;
                    }
                    if (*special != '\\') {
                        break;
                    }
                }
                strncpy(linePtr, start, special - start);
                linePtr += special - start;
                start = special;
            }
            if (*special == '"') {
                strncpy(linePtr, start, special - start);
                linePtr += special - start;
                *linePtr++ = '\\';
                *linePtr++ = '"';
                start = special + 1;
            }
            if (*special == '\0') {
                break;
            }
            special++;
        }
        strncpy(linePtr, start, special - start);
        linePtr += special - start;
        if (quote) {
            *linePtr++ = '"';
        }
    }
    *linePtr = 0;
}

/*
 *----------------------------------------------------------------------
 *
 * MakeTempFile --
 *
 *	Helper function for CreateProcess under Win32s.  Makes a 
 *	temporary file that _won't_ go away automatically when it's file
 *	handle is closed.  Used for simulated pipes, which are written
 *	in one pass and reopened and read in the next pass.
 *
 * Results:
 *	namePtr is filled with the name of the temporary file.
 *
 * Side effects:
 *	A temporary file with the name specified by namePtr is created.  
 *	The caller is responsible for deleting this temporary file.
 *
 *----------------------------------------------------------------------
 */

static char *
MakeTempFile(char *namePtr)
{
    if ((GetTempPath(MAX_PATH, namePtr) == 0)
        || (GetTempFileName(namePtr, "COD", 0, namePtr) == 0)) {
        return NULL;
    }

    return namePtr;
}

/*
 *----------------------------------------------------------------------
 *
 * CopyChannel --
 *
 *	Helper function used by CodeCreateProcess under Win32s.  Copies
 *	what remains of source file to destination file; source file 
 *	pointer need not be positioned at the beginning of the file if
 *	all of source file is not desired, but data is copied up to end 
 *	of source file.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static void
CopyChannel(HANDLE dst, HANDLE src)
{
    char buf[8192];
    DWORD dwRead, dwWrite;

    while (ReadFile(src, buf, sizeof(buf), &dwRead, NULL) != FALSE) {
        if (dwRead == 0) {
            break;
        }
        if (WriteFile(dst, buf, dwRead, &dwWrite, NULL) == FALSE) {
            break;
        }
    }
}
/*
 *----------------------------------------------------------------------
 *
 * HasConsole --
 *
 *	Determines whether the current application is attached to a
 *	console.
 *
 * Results:
 *	Returns TRUE if this application has a console, else FALSE.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static BOOL
HasConsole(void)
{
    HANDLE handle = CreateFile("CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE,
                               NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
        return TRUE;
    } else {
        return FALSE;
    }
}
/*
 *----------------------------------------------------------------------
 *
 * CodeCreateProcess --
 *
 *	Create a child process that has the specified files as its 
 *	standard input, output, and error.  The child process runs
 *	synchronously under Win32s and asynchronously under Windows NT
 *	and Windows 95, and runs with the same environment variables
 *	as the creating process.
 *
 *	The complete Windows search path is searched to find the specified 
 *	executable.  If an executable by the given name is not found, 
 *	automatically tries appending ".com", ".exe", and ".bat" to the 
 *	executable name.
 *
 * Results:
 *	The return value is -1 and an error message is left in
 *	interp->result if there was a problem creating the child 
 *	process.  Otherwise, the return value is 0 and *pidPtr is
 *	filled with the process id of the child process.
 * 
 * Side effects:
 *	A process is created.
 *	
 *----------------------------------------------------------------------
 */

int
CodeCreateProcess(int argc, const char **argv, HANDLE inputFile, HANDLE outputFile, HANDLE errorFile, 
                  HANDLE *pidPtr)
/* If this procedure is successful, pidPtr
 * is filled with the process id of the child
 * process. */
{
    int result, applType, createFlags;
    char  command[STRINGMAX];
    char  *cmdLine;
    STARTUPINFO startInfo;
    PROCESS_INFORMATION procInfo;
    SECURITY_ATTRIBUTES secAtts;
    HANDLE hProcess, h, inputHandle, outputHandle, errorHandle;
    char execPath[MAX_PATH];
    const char *originalName;
    HANDLE filePtr;
    OSVERSIONINFO os;
    DWORD PlatformId;
    char *comspec;

    os.dwOSVersionInfoSize = sizeof(os);
    GetVersionEx(&os);
    PlatformId = os.dwPlatformId;

    cmdLine = command;
    cmdLine[0] = 0;

    applType = ApplicationType(argv[0], execPath);
    comspec = NULL;
    if (applType == APPL_NONE && (comspec = getenv("COMSPEC"))) {
        // not found, try COMSPEC/C

        applType = ApplicationType(comspec, execPath);
        if (applType != APPL_NONE) {
            strcpy(cmdLine, comspec);
            strcat(cmdLine, " /C ");
            cmdLine += strlen(cmdLine);
            strcpy(execPath, argv[0]);
        }
    }

    if (applType != APPL_DOS && applType != APPL_WIN32) {
        return -1;

    }
    originalName = argv[0];
    argv[0] = execPath;

    result = -1;

    if (PlatformId == VER_PLATFORM_WIN32s) {
        /*
         * Under Win32s, there are no pipes.  In order to simulate pipe
         * behavior, the child processes are run synchronously and their
         * I/O is redirected from/to temporary files before the next 
         * stage of the pipeline is started.
         */

        MSG msg;
        DWORD status;
        char *inputFileName, *outputFileName;
        char inputTempFile[MAX_PATH];
        char outputTempFile[MAX_PATH];

        BuildCommandLine(argc, argv, cmdLine);

        ZeroMemory(&startInfo, sizeof(startInfo));
        startInfo.cb = sizeof(startInfo);

        inputTempFile[0] = outputTempFile[0] = 0;
        outputHandle = INVALID_HANDLE_VALUE;

        inputFileName = NULL;
        outputFileName = NULL;
        if (inputFile != NULL) {
            filePtr = inputFile;
            h = INVALID_HANDLE_VALUE;
            inputFileName = MakeTempFile(inputTempFile);
            if (inputFileName != NULL) {
                h = CreateFile(inputFileName, GENERIC_WRITE, 0, 
                               NULL, CREATE_ALWAYS, 0, NULL);
            }
            if (h == INVALID_HANDLE_VALUE) {
                goto end32s;
            }
            CopyChannel(h, filePtr);
            CloseHandle(h);
        }
        if (inputFileName == NULL) {
            inputFileName = "nul";
        }
        if (outputFile != NULL) {
            filePtr = outputFile;
            outputFileName = MakeTempFile(outputTempFile);
            if (outputFileName == NULL) {
                goto end32s;
            }
            outputHandle = filePtr;
        }
        if (outputFileName == NULL) {
            outputFileName = "nul";
        }

        if (CreateProcess(NULL, command, NULL, NULL, 
                          FALSE, DETACHED_PROCESS, NULL, NULL, &startInfo, 
                          &procInfo) != 0) {
            CloseHandle(procInfo.hThread);
            while (1) {
                if (GetExitCodeProcess(procInfo.hProcess, &status) == FALSE) {
                    break;
                }
                if (status != STILL_ACTIVE) {
                    break;
                }
                if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            *pidPtr = procInfo.hProcess;
            result = 0;
        }

        end32s:
        if (outputHandle != INVALID_HANDLE_VALUE) {
            /*
             * Now copy stuff from temp file to actual output handle. Don't
             * close outputHandle because it is associated with the output
             * file owned by the caller.
             */

            h = CreateFile(outputFileName, GENERIC_READ, 0, NULL, OPEN_ALWAYS,
                           0, NULL);
            if (h != INVALID_HANDLE_VALUE) {
                CopyChannel(outputHandle, h);
            }
            CloseHandle(h);
        }

        if (inputFileName == inputTempFile) {
            DeleteFile(inputFileName);
        }

        if (outputFileName == outputTempFile) {
            DeleteFile(outputFileName);
        }

        return result;
    }
    hProcess = GetCurrentProcess();

    /*
     * STARTF_USESTDHANDLES must be used to pass handles to child process.
     * Using SetStdHandle() and/or dup2() only works when a console mode 
     * parent process is spawning an attached console mode child process.
     */

    ZeroMemory(&startInfo, sizeof(startInfo));
    startInfo.cb = sizeof(startInfo);
    startInfo.dwFlags   = STARTF_USESTDHANDLES;
    startInfo.hStdInput = INVALID_HANDLE_VALUE;
    startInfo.hStdOutput= INVALID_HANDLE_VALUE;
    startInfo.hStdError = INVALID_HANDLE_VALUE;

    secAtts.nLength = sizeof(SECURITY_ATTRIBUTES);
    secAtts.lpSecurityDescriptor = NULL;
    secAtts.bInheritHandle = TRUE;

    /*
     * We have to check the type of each file, since we cannot duplicate 
     * some file types.  
     */

    inputHandle = INVALID_HANDLE_VALUE;
    if (inputFile != NULL) {
        filePtr = inputFile;
        inputHandle = filePtr;
    }
    outputHandle = INVALID_HANDLE_VALUE;
    if (outputFile != NULL) {
        filePtr = outputFile;
        outputHandle = filePtr;
    }
    errorHandle = INVALID_HANDLE_VALUE;
    if (errorFile != NULL) {
        filePtr = errorFile;
        errorHandle = filePtr;
    }

    /*
     * Duplicate all the handles which will be passed off as stdin, stdout
     * and stderr of the child process. The duplicate handles are set to
     * be inheritable, so the child process can use them.
     */

    if (inputHandle == INVALID_HANDLE_VALUE) {
        /* 
         * If handle was not set, stdin should return immediate EOF.
         * Under Windows95, some applications (both 16 and 32 bit!) 
         * cannot read from the NUL device; they read from console
         * instead.  When running tk, this is fatal because the child 
         * process would hang forever waiting for EOF from the unmapped 
         * console window used by the helper application.
         *
         * Fortunately, the helper application detects a closed pipe 
         * as an immediate EOF and can pass that information to the 
         * child process.
         */

        if (CreatePipe(&startInfo.hStdInput, &h, &secAtts, 0) != FALSE) {
            CloseHandle(h);
        }
    } else {
        DuplicateHandle(hProcess, inputHandle, hProcess, &startInfo.hStdInput,
                        0, TRUE, DUPLICATE_SAME_ACCESS);
    }
    if (startInfo.hStdInput == INVALID_HANDLE_VALUE) {
        goto end;
    }

    if (outputHandle == INVALID_HANDLE_VALUE) {
        /*
         * If handle was not set, output should be sent to an infinitely 
         * deep sink.  Under Windows 95, some 16 bit applications cannot
         * have stdout redirected to NUL; they send their output to
         * the console instead.  Some applications, like "more" or "dir /p", 
         * when outputting multiple pages to the console, also then try and
         * read from the console to go the next page.  When running tk, this
         * is fatal because the child process would hang forever waiting
         * for input from the unmapped console window used by the helper
         * application.
         *
         * Fortunately, the helper application will detect a closed pipe
         * as a sink.
         */

        startInfo.hStdOutput = CreateFile("NUL:", GENERIC_WRITE, 0,
                                          &secAtts, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    } else {
        DuplicateHandle(hProcess, outputHandle, hProcess, &startInfo.hStdOutput, 
                        0, TRUE, DUPLICATE_SAME_ACCESS);
    }
    if (startInfo.hStdOutput == INVALID_HANDLE_VALUE) {
        goto end;
    }

    if (errorHandle == INVALID_HANDLE_VALUE) {
        /*
         * If handle was not set, errors should be sent to an infinitely
         * deep sink.
         */

        startInfo.hStdError = CreateFile("NUL:", GENERIC_WRITE, 0,
                                         &secAtts, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    } else {
        DuplicateHandle(hProcess, errorHandle, hProcess, &startInfo.hStdError, 
                        0, TRUE, DUPLICATE_SAME_ACCESS);
    } 
    if (startInfo.hStdError == INVALID_HANDLE_VALUE) {
        goto end;
    }
    /* 
     * If we do not have a console window, then we must run DOS and
     * WIN32 console mode applications as detached processes. This tells
     * the loader that the child application should not inherit the
     * console, and that it should not create a new console window for
     * the child application.  The child application should get its stdio 
     * from the redirection handles provided by this application, and run
     * in the background.
     *
     * If we are starting a GUI process, they don't automatically get a 
     * console, so it doesn't matter if they are started as foreground or
     * detached processes.  The GUI window will still pop up to the
     * foreground.
     */

    if (PlatformId == VER_PLATFORM_WIN32_NT) {
        if (HasConsole()) {
            createFlags = 0;
        } else {
            createFlags = DETACHED_PROCESS|CREATE_NEW_PROCESS_GROUP;
        } 
    } else {
        if (HasConsole()) {
            createFlags = 0;
        } else {
            createFlags = DETACHED_PROCESS;
        }
    }

    /*
     * cmdLine gets the full command line used to invoke the executable,
     * including the name of the executable itself.  The command line
     * arguments in argv[] are stored in cmdLine separated by spaces. 
     * Special characters in individual arguments from argv[] must be 
     * quoted when being stored in cmdLine.
     *
     * When calling any application, bear in mind that arguments that 
     * specify a path name are not converted.  If an argument contains 
     * forward slashes as path separators, it may or may not be 
     * recognized as a path name, depending on the program.  In general,
     * most applications accept forward slashes only as option 
     * delimiters and backslashes only as paths.
     *
     * Additionally, when calling a 16-bit dos or windows application, 
     * all path names must use the short, cryptic, path format (e.g., 
     * using ab~1.def instead of "a b.default").  
     */

    BuildCommandLine(argc, argv, cmdLine);

    if (!CreateProcess(NULL, command, NULL, NULL, TRUE, 
                       createFlags, NULL, NULL, &startInfo, &procInfo)) {
        goto end;
    }

    /*
     * This wait is used to force the OS to give some time to the DOS
     * process.
     */

    if (applType == APPL_DOS) {
        WaitForSingleObject(procInfo.hProcess, 50);
    }

    /* 
     * "When an application spawns a process repeatedly, a new thread 
     * instance will be created for each process but the previous 
     * instances may not be cleaned up.  This results in a significant 
     * virtual memory loss each time the process is spawned.  If there 
     * is a WaitForInputIdle() call between CreateProcess() and
     * CloseHandle(), the problem does not occur." PSS ID Number: Q124121
     */

    WaitForInputIdle(procInfo.hProcess, 5000);
    CloseHandle(procInfo.hThread);

    *pidPtr = procInfo.hProcess;
    result = 0;

    end:
    if (startInfo.hStdInput != INVALID_HANDLE_VALUE) {
        CloseHandle(startInfo.hStdInput);
    }
    if (startInfo.hStdOutput != INVALID_HANDLE_VALUE) {
        CloseHandle(startInfo.hStdOutput);
    }
    if (startInfo.hStdError != INVALID_HANDLE_VALUE) {
        CloseHandle(startInfo.hStdError);
    }
    return result;
}
#endif
/*                                                                      */
/*      cexecvp - execute a binary from a standard place		*/
/*                                                                      */

int
cexecvp(const char *path, const char **list)
{
#if OSWIN32
    HANDLE pid;
    int argc;
    DWORD ret;

    for (argc = 0; list[argc]; ++argc)
        continue;
    fflush(stdout);
    fflush(stderr);
    if (CodeCreateProcess(argc, list,
                          (HANDLE)_get_osfhandle(fileno(stdin)),
                          (HANDLE)_get_osfhandle(fileno(stdout)),
                          (HANDLE)_get_osfhandle(fileno(stderr)),
                          &pid) == -1)
        return -1;

    ret = WaitForSingleObject(pid, INFINITE);
    if (ret != WAIT_FAILED)
        GetExitCodeProcess(pid, &ret);
    else
        ret = 1;
    CloseHandle(pid);
    exit(ret);
#else
    return -1;
#endif
}

#endif

};

