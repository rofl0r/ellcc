/** The C startup function.
 * This function is called by _start(), the program entry point.
 * See start.h.
 */
#include <stdlib.h>             // exit()
#include <string.h>             // strrchr()

static char *initial_env[] = { 0 };

/* Posix says `environ' is a pointer to a null terminated list of pointers.
   Hence `environ' itself is never NULL.  */
char **environ = &initial_env[0];
char *__progname;

extern int main(int argc, char** argv, char** environ);

extern char __bss_start;
extern char _end;

void _estart(int argc, char** argv, char** _environ)
{
    if (argv[0] != NULL) {
        __progname = strrchr(argv[0], '/');
        if (__progname == NULL) {
            __progname = argv[0];
        }
    }
    environ = _environ;
    memset(&__bss_start, 0, &_end - &__bss_start);
    int status = main(argc, argv, environ);
    exit(status);
}
