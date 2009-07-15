#include <sysdep.h>
#include <abort-instr.h>
#include <libc-symbols.h>

void _exit(int status)
{
    for ( ;; ) {
#ifdef __NR_exit_group
        INLINE_SYSCALL(exit_group, 1, status);
#endif
        INLINE_SYSCALL(exit, 1, status);

#ifdef ABORT_INSTRUCTION
        ABORT_INSTRUCTION;
#endif
    }
}

libc_hidden_def(_exit)
weak_alias(_exit, _Exit)
