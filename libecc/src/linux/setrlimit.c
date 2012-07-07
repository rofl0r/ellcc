#include <syscall.h>
#include <sys/resource.h>

int setrlimit(int resource, const struct rlimit *rlim)
{
    return INLINE_SYSCALL(setrlimit, 2, resource, rlim);
}
