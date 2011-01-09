/** The C startup function.
 * This function is called by _start(), the program entry point.
 * See start.h.
 */
#include <stdlib.h>             // exit()
#include <string.h>             // strrchr()
#include <asm/_start.h>         // _start()

static char *initial_env[] = { 0 };

/* Posix says `environ' is a pointer to a null terminated list of pointers.
   Hence `environ' itself is never NULL.  */
char **environ = &initial_env[0];
char *__progname;

extern int main(int argc, char** argv, char** environ);

// RICH: extern char __bss_end__;
// RICH: extern char __bss_start__;

void _estart(int argc, char** argv, char** _environ)
{
    if (argv[0] != NULL) {
        __progname = strrchr(argv[0], '/');
        if (__progname == NULL) {
            __progname = argv[0];
        }
    }
    environ = _environ;
// RICH:     memset(&__bss_start__, 0, &__bss_end__ - &__bss_start__);
    int status = main(argc, argv, environ);
    exit(status);
}
