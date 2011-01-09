#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

pid_t getgid(void)
{
    return INLINE_SYSCALL(getgid, 0);
}
