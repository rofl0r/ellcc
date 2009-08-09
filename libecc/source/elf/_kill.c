#include <sysdep.h>
#include <sys/types.h>
#include <signal.h>
#include <libc-symbols.h>

int _kill(pid_t pid, int sig)
{
    return INLINE_SYSCALL(kill, 2, pid, sig);
}
