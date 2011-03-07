#include <syscall.h>
#include <sys/resource.h>

int getrlimit(int resource,  struct rlimit *rlim)
{
    return INLINE_SYSCALL(getrlimit, 2, resource, rlim);
}
