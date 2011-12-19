#include <syscall.h>
#include <unistd.h>

pid_t vfork(void)
{
#if defined(__NR_vfork)
    return INLINE_SYSCALL(vfork, 0);
#else
    return INLINE_SYSCALL(fork, 0);
#endif
}
