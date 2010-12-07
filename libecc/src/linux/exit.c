#include <sysdep.h>
#include <abort-instr.h>

void _exit(int status)
{
    // Get rid of the compiler warning about noreturn functions returning.
    for ( ;; ) {
#ifdef __NR_exit_group
        (void)INLINE_SYSCALL(exit_group, 1, status);
#endif
        (void)INLINE_SYSCALL(exit, 1, status);
#ifdef ABORT_INSTRUCTION
        ABORT_INSTRUCTION;
#endif
    }
}
