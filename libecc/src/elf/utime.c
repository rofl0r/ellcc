#include <sysdep.h>
#include <unistd.h>
#include <sys/types.h>
#include <utime.h>

int utime(const char* filename, const struct utimbuf* times)
{
    struct timeval tvs[2];
    struct timeval *tp;

    if (times != NULL) {
        // A time was given.
        tvs[0].tv_sec = (time_t)times->actime;
        tvs[0].tv_usec = 0L;
        tvs[1].tv_sec = (time_t)times->modtime;
        tvs[1].tv_usec = 0L;
        tp = tvs;
    } else {
        // Use the current time.
        tp = NULL;
    }

  return utimes(filename, tp);
}
