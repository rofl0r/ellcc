/*
FUNCTION
<<__env_lock>>, <<__env_unlock>>--lock environ variable

INDEX
	__env_lock
INDEX
	__env_unlock

ANSI_SYNOPSIS
	#include "envlock.h"
       void __env_lock (struct _reent *<[reent]>);
       void __env_unlock (struct _reent *<[reent]>);

TRAD_SYNOPSIS
	void __env_lock(<[reent]>)
       struct _reent *<[reent]>;

	void __env_unlock(<[reent]>)
       struct _reent *<[reent]>;

DESCRIPTION
The <<setenv>> family of routines call these functions when they need
to modify the environ variable.  The version of these routines supplied
in the library does not do anything.  If multiple threads of execution
can call <<setenv>>, or if <<setenv>> can be called reentrantly, then
you need to define your own versions of these functions in order to
safely lock the memory pool during a call.  If you do not, the memory
pool may become corrupted.

A call to <<setenv>> may call <<__env_lock>> recursively; that is,
the sequence of calls may go <<__env_lock>>, <<__env_lock>>,
<<__env_unlock>>, <<__env_unlock>>.  Any implementation of these
routines must be careful to avoid causing a thread to wait for a lock
that it already holds.
*/

/* The following lines are copied and pasted from
   envlock.h */
#include <_ansi.h>
#include <sys/reent.h>

#define ENV_LOCK __env_lock(reent_ptr)
#define ENV_UNLOCK __env_unlock(reent_ptr)

void _EXFUN(__env_lock,(struct _reent *reent));
void _EXFUN(__env_unlock,(struct _reent *reent));
/* end copy and paste */



void
__env_lock (ptr)
     struct _reent *ptr;
{
}

void
__env_unlock (ptr)
     struct _reent *ptr;
{
}
