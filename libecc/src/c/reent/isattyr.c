/* Reentrant versions of isatty system call.  */

#include <config.h>
#include <reent.h>
#include <unistd.h>
#include <_syslist.h>

/* Some targets provides their own versions of these functions.  Those
   targets should define REENTRANT_SYSCALLS_PROVIDED in TARGET_CFLAGS.  */

#ifndef REENTRANT_SYSCALLS_PROVIDED

/* We use the errno variable used by the system dependent layer.  */
#undef errno
extern int errno;

/*
FUNCTION
	<<_isatty_r>>---Reentrant version of isatty
	
INDEX
	_isatty_r

ANSI_SYNOPSIS
	#include <reent.h>
	int _isatty_r(struct _reent *<[ptr]>,
		     int <[fd]>);

TRAD_SYNOPSIS
	#include <reent.h>
	int _isatty_r(<[ptr]>, <[fd]>)
	struct _reent *<[ptr]>;
	int <[fd]>;

DESCRIPTION
	This is a reentrant version of <<isatty>>.  It
	takes a pointer to the global data block, which holds
	<<errno>>.
*/

int
_isatty_r (ptr, fd)
     struct _reent *ptr;
     int fd;
{
  int ret;

  errno = 0;
  if ((ret = _isatty (fd)) == -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}

#endif /* ! defined (REENTRANT_SYSCALLS_PROVIDED) */
