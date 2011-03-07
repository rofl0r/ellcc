#include <syscall.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

pid_t wait4(pid_t pid, int *status, int options, struct rusage *rusage)
{
    return INLINE_SYSCALL(wait4, 4, (int)pid, status, options, rusage);
}
