#include <syscall.h>
#include <unistd.h>

pid_t vfork(void)
{
    return INLINE_SYSCALL(vfork, 0);
}
