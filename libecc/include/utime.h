#ifndef _UTIME_H_
#define _UTIME_H_
_BEGIN_STD_C

#include <sys/utime.h>
#include <sys/time.h>

extern int utime(const char* filename, const struct utimbuf* times);
extern int utimes(const char *filename, const struct timeval times[2]);

_END_STD_C
#endif
