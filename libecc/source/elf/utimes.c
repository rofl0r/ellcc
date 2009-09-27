#include <sysdep.h>
#include <unistd.h>
#include <sys/types.h>
#include <utime.h>
#include <errno.h>

int utimes(const char *filename, const struct timeval times[2])
{
#ifdef __NR_utimes
    int result = INLINE_SYSCALL (utimes, 2, filename, times);
    if (result != -1 || errno != ENOSYS) {
        return result;
    }
#else
    struct utimbuf *tp;
    struct utimbuf tvs;

    if (tvp != NULL) {
        tp = &tvs;
        tvs.actime = times[0].tv_sec + times[0].tv_usec / 1000000;
        tvs.modtime = times[1].tv_sec + times[1].tv_usec / 1000000;
    } else {
        tp = NULL;
    }

    return INLINE_SYSCALL (utime, 2, filename, tp);
#endif
}
