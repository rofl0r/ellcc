/* Reentrant versions of fstat system call.  This implementation just
   calls the fstat system call.  */

#include <config.h>
#include <reent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <_syslist.h>

/* Some targets provides their own versions of these functions.  Those
   targets should define REENTRANT_SYSCALLS_PROVIDED in TARGET_CFLAGS.  */

#ifndef REENTRANT_SYSCALLS_PROVIDED

/* We use the errno variable used by the system dependent layer.  */
#undef errno
extern int errno;

/*
FUNCTION
	<<_fstat_r>>---Reentrant version of fstat
	
INDEX
	_fstat_r

ANSI_SYNOPSIS
	#include <reent.h>
	int _fstat_r(struct _reent *<[ptr]>,
		     int <[fd]>, struct stat *<[pstat]>);

TRAD_SYNOPSIS
	#include <reent.h>
	int _fstat_r(<[ptr]>, <[fd]>, <[pstat]>)
	struct _reent *<[ptr]>;
	int <[fd]>;
	struct stat *<[pstat]>;

DESCRIPTION
	This is a reentrant version of <<fstat>>.  It
	takes a pointer to the global data block, which holds
	<<errno>>.
*/

int
_fstat_r (ptr, fd, pstat)
     struct _reent *ptr;
     int fd;
     struct stat *pstat;
{
  int ret;

  errno = 0;
  if ((ret = _fstat (fd, pstat)) == -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}

#endif /* ! defined (REENTRANT_SYSCALLS_PROVIDED) */
