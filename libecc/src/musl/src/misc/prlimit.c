#include <unistd.h>
#include <sys/resource.h>
#include "syscall.h"
#include <errno.h>

int prlimit(pid_t pid, int resource, const struct rlimit *new_limit, struct rlimit *old_limit)
{
#if !defined(SYS_prlimit64)
        errno = ENOSYS;
        return -1;
#else
	return syscall(SYS_prlimit64, pid, resource, new_limit, old_limit);
#endif
}
