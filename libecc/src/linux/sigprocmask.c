#include <syscall.h>
#include <signal.h>

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
    return INLINE_SYSCALL(rt_sigprocmask, 4, how, set, oldset, _NSIG / 8);
}
