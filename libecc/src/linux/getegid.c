#include <syscall.h>
#include <sys/types.h>

pid_t getegid(void)
{
    return INLINE_SYSCALL(getegid, 0);
}
