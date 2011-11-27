#include <syscall.h>
#include <abort-instr.h>

void _exit(int status)
{
    // Get rid of the compiler warning about noreturn functions returning.
    for ( ;; ) {
    }
}
