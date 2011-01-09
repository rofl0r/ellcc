#include <syscall.h>
#include <signal.h>

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
    return INLINE_SYSCALL(sigprocmask, 3, how, set, oldset);
}
