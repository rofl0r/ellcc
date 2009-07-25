#ifndef _SIGNAL_H_
#define _SIGNAL_H_

_BEGIN_STD_C

#include <sys/signal.h>

typedef int	sig_atomic_t;		/* Atomic entity type (ANSI) */

#define SIG_DFL ((_sig_func_ptr)0)	/* Default action */
#define SIG_IGN ((_sig_func_ptr)1)	/* Ignore action */
#define SIG_ERR ((_sig_func_ptr)-1)	/* Error return */

struct _reent;

_sig_func_ptr _signal_r(struct _reent *, int, _sig_func_ptr);
int _raise_r(struct _reent *, int);

#ifndef _REENT_ONLY
_sig_func_ptr signal(int, _sig_func_ptr);
int raise(int);
#endif

_END_STD_C

#endif
