#include <syscall.h>
#include <sys/types.h>

pid_t _getpid(void)
{
    return INLINE_SYSCALL(getpid, 0);
}
