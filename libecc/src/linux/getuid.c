#include <syscall.h>
#include <sys/types.h>

pid_t getuid(void)
{
    return INLINE_SYSCALL(getuid, 0);
}
