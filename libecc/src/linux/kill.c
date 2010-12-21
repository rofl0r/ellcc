#include <syscall.h>
#include <sys/types.h>
#include <signal.h>

int kill(pid_t pid, int sig)
{
    return INLINE_SYSCALL(kill, 2, pid, sig);
}
