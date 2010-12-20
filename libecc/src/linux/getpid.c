#include <syscall.h>
#include <sys/types.h>

pid_t getpid(void)
{
    return INLINE_SYSCALL(getpid, 0);
}
