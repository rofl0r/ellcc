#include <stdlib.h>             // exit()
#include <string.h>             // memset()
#include <stdarg.h>

extern char **environ;
extern int main();

extern char __bss_end__;
extern char __bss_start__;

void _start(char* first, ...)
{
    va_list ap;
    va_start(ap, first);
    int argc = *(int*)((char*)ap - sizeof(char*) - sizeof(int));
    char **argv = (char*)ap - sizeof(char*);
    va_end(ap);
    environ = argv + argc + 1;
    memset(&__bss_start__, 0, &__bss_end__ - &__bss_start__);
    int status = main(argc, argv, environ);
    exit(status);
}
