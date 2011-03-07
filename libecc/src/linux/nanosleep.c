#include <syscall.h>
#include <time.h>

int nanosleep(const struct timespec *req, struct timespec *rem)
{
    return INLINE_SYSCALL(nanosleep, 2, req, rem);
}
