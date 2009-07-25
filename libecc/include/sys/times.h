#ifndef	_SYS_TIMES_H
#define	_SYS_TIMES_H

_BEGIN_STD_C
#include <machine/types.h>

#ifndef __clock_t_defined
typedef _CLOCK_T_ clock_t;
#define __clock_t_defined
#endif

/*  Get Process Times, P1003.1b-1993, p. 92 */
struct tms {
	clock_t	tms_utime;		/* user time */
	clock_t	tms_stime;		/* system time */
	clock_t	tms_cutime;		/* user time, children */
	clock_t	tms_cstime;		/* system time, children */
};

clock_t times(struct tms *);
_END_STD_C

#endif	/* !_SYS_TIMES_H */
