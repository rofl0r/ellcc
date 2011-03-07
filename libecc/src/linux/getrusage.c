#include <syscall.h>
#include <sys/resource.h>

int getrusage(int who,  struct rusage *usage)
{
    return INLINE_SYSCALL(getrusage, 2, who, usage);
}
