#include <syscall.h>
#include <signal.h>

int sigsuspend(const sigset_t *mask)
{
    return INLINE_SYSCALL(sigsuspend, 2, mask, _NSIG / 8);
}
