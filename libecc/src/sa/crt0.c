/** The C startup function.
 * This function is called by _start(), the program entry point.
 * See start.S.
 */
#include <stdlib.h>             // exit()
#include <string.h>             // strrchr()

static char *initial_env[] = { 0 };

/* Posix says `environ' is a pointer to a null terminated list of pointers.
 * Hence `environ' itself is never NULL.
 */
char **environ = &initial_env[0];

// A default value for argv.
 
char *__progname;

extern int main(int argc, char **argv, char **environ);
extern void _init(void);

extern char __bss_start[];              // The start of the unitialized data section.
extern char _end[];                     // The end of the unitialized data section.
extern char _data_image_begin[];        // The start of the non-const initialized data section in flash.
extern char _data_begin[];              // The start of the non-const initialized data section in RAM.
extern char _data_end[];                // The end of the non-const initialized data section in RAM.

void _estart(int argc, char **argv, char **_environ)
{
    char *_argv[2];
    if (argc && argv[0] != NULL) {
        __progname = strrchr(argv[0], '/');
        if (__progname == NULL) {
            __progname = argv[0];
        } else {
            ++__progname;
        }
    } else {
        _argv[0] = "main";
        _argv[1] = NULL;
        argv = _argv;
        __progname = argv[0];
    }

    if (_environ) {
        environ = _environ;
    }
 
    // Clear the uninitialized data section.
    memset(__bss_start, 0, _end - __bss_start);

    // Initialize the non-const initialized data section.
    memcpy(_data_begin, _data_image_begin, _data_end - _data_begin);

    _init();            // Call the constructors.
    int status = main(argc, argv, environ);
    exit(status);
}
