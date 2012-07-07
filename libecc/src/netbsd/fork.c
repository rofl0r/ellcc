#include <syscall.h>
#include <unistd.h>

pid_t fork(void)
{
    return INLINE_SYSCALL(fork, 0);
}
