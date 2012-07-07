#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

int setpgid(pid_t pid, pid_t pgid)
{
    return INLINE_SYSCALL(setpgid, 2, pid, pgid);
}
