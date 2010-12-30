#include <syscall.h>
#include <sys/types.h>

pid_t geteuid(void)
{
    return INLINE_SYSCALL(geteuid, 0);
}
