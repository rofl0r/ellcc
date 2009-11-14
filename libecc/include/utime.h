#ifndef _UTIME_H_
#define _UTIME_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/utime.h>
#include <sys/time.h>

extern int utime(const char* filename, const struct utimbuf* times);
extern int utimes(const char *filename, const struct timeval times[2]);

#ifdef __cplusplus
}
#endif
#endif
