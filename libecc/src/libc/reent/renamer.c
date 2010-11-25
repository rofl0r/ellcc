/* Reentrant version of rename system call.  */

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
	<<_rename_r>>---Reentrant version of rename
	
INDEX
	_rename_r

ANSI_SYNOPSIS
	#include <reent.h>
	int _rename_r(struct _reent *<[ptr]>,
		     int <[fd]>, int <[cmd]>, <[arg]>);

TRAD_SYNOPSIS
	#include <reent.h>
	int _rename_r(<[ptr]>, <[fd]>, <[cmd]>, <[arg]>)
	struct _reent *<[ptr]>;
	int <[fd]>;
	int <[cmd]>;
	int <[arg]>;

DESCRIPTION
	This is a reentrant version of <<rename>>.  It
	takes a pointer to the global data block, which holds
	<<errno>>.
*/

int _rename_r(struct _reent *ptr, const char *old, const char *new)
{
  int ret = 0;

  errno = 0;
  if ((ret = _rename (old, new)) == -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}

#endif /* ! defined (REENTRANT_SYSCALLS_PROVIDED) */
