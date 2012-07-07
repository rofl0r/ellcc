/** The C startup function.
 * This function is called by _start(), the program entry point.
 * See start.S.
 */
#include <stdlib.h>             // exit()
#include <string.h>             // strrchr()

char **environ;
char *__progname;

extern int main(int argc, char **argv, char **environ);
extern void _init(void);

void _estart(int argc, char **argv, char **_environ)
{
    if (argv[0] != NULL) {
        __progname = strrchr(argv[0], '/');
        if (__progname == NULL) {
            __progname = argv[0];
        } else {
            ++__progname;
        }
    }
    environ = _environ;
    _init();            // Call the constructors.
    int status = main(argc, argv, environ);
    exit(status);
}
