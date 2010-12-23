#include <syscall.h>
#include <signal.h>
#include <unistd.h>

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
    return INLINE_SYSCALL(sigaction, 3, signum, act, oldact);
}
