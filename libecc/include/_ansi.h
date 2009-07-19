#ifndef	__ANSI_H_
#define	__ANSI_H_

#include <sys/config.h>

/* First try to figure out whether we really are in an ANSI C environment.  */
/* FIXME: This probably needs some work.  Perhaps sys/config.h can be
   prevailed upon to give us a clue.  */

#ifdef __STDC__
#define _HAVE_STDC
#endif

/*  ISO C++.  */
#ifdef __cplusplus
#define _BEGIN_STD_C extern "C" {
#define _END_STD_C  }
#else
#define _BEGIN_STD_C
#define _END_STD_C
#endif

#endif
