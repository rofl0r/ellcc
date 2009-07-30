#include <stdlib.h>             // exit()
#include <string.h>             // memset()

extern char **environ;
extern int main(int argc,char **argv,char **envp);

extern char __bss_end__;
extern char __bss_start__;

void _start(int arguments)
{
    int *p = &arguments-1;
    int argc = *p;
    char **argv = (char **)(p + 1);
    environ = argv + argc + 1;
    memset(&__bss_start__, 0, &__bss_end__ - &__bss_start__);
    int status = main(argc, argv, environ);
    exit(status);
}
