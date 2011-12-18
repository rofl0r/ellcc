#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>

int lutimes(const char *filename, const struct timeval times[2])
{
    struct timespec ts[2];
    ts[0].tv_sec = times[0].tv_sec;
    ts[0].tv_nsec = times[0].tv_usec * 1000;
    ts[1].tv_sec = times[1].tv_sec;
    ts[1].tv_nsec = times[1].tv_usec * 1000;
    // RICH: -11 is AT_FDCWD.
    return INLINE_SYSCALL(utimensat, 3, -100, filename, times);
}
