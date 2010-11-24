/** The C startup function.
 * This function is called by _start(), the program entry point.
 * See start.h.
 */
#include <stdlib.h>             // exit()
#include <string.h>             // memset()
#include <stdarg.h>
#include <asm/_start.h>         // _start()

extern char **environ;
extern int main(int argc, char** argv, char** environ);

extern char __bss_end__;
extern char __bss_start__;

void _estart(int argc, char** argv, char** _environ)
{
    environ = _environ;
    memset(&__bss_start__, 0, &__bss_end__ - &__bss_start__);
    int status = main(argc, argv, environ);
    exit(status);
}
