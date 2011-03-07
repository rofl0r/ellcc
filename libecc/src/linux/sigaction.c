#include <syscall.h>
#include <signal.h>

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
    return INLINE_SYSCALL(sigaction, 4, signum, act, oldact, _NSIG / 8);
}
