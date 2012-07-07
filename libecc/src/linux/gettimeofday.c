#include <syscall.h>
#include <sys/time.h>
#include <unistd.h>

int gettimeofday(struct timeval * restrict tv, void * restrict tz)
{
    return INLINE_SYSCALL(gettimeofday, 2, tv, tz);
}
