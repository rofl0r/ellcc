#ifndef _DIRENT_H_
#define _DIRENT_H_
#include <_ansi.h>
_BEGIN_STD_C
#include <sys/dirent.h>

#if !defined(MAXNAMLEN) && !defined(_POSIX_SOURCE)
#define MAXNAMLEN 1024
#endif

_END_STD_C
#endif
