#ifndef _CTYPE_H_
#define _CTYPE_H_

#include "_ansi.h"
_BEGIN_STD_C

int isalnum(int __c);
int isalpha(int __c);
int iscntrl(int __c);
int isdigit(int __c);
int isgraph(int __c);
int islower(int __c);
int isprint(int __c);
int ispunct(int __c);
int isspace(int __c);
int isupper(int __c);
int isxdigit(int __c);
int tolower(int __c);
int toupper(int __c);

#ifndef __STRICT_ANSI__
int isblank(int __c);
int isascii(int __c);
int toascii(int __c);
int _tolower(int __c);
int _toupper(int __c);
#endif

#define	_U	01
#define	_L	02
#define	_N	04
#define	_S	010
#define _P	020
#define _C	040
#define _X	0100
#define	_B	0200

extern	const char	*__ctype_ptr;
extern	const char	_ctype_[];  /* For backward compatibility.  */

#ifndef __cplusplus
#define	isalpha(c)	((__ctype_ptr)[(unsigned)(c)]&(_U|_L))
#define	isupper(c)	((__ctype_ptr)[(unsigned)(c)]&_U)
#define	islower(c)	((__ctype_ptr)[(unsigned)(c)]&_L)
#define	isdigit(c)	((__ctype_ptr)[(unsigned)(c)]&_N)
#define	isxdigit(c)	((__ctype_ptr)[(unsigned)(c)]&(_X|_N))
#define	isspace(c)	((__ctype_ptr)[(unsigned)(c)]&_S)
#define ispunct(c)	((__ctype_ptr)[(unsigned)(c)]&_P)
#define isalnum(c)	((__ctype_ptr)[(unsigned)(c)]&(_U|_L|_N))
#define isprint(c)	((__ctype_ptr)[(unsigned)(c)]&(_P|_U|_L|_N|_B))
#define	isgraph(c)	((__ctype_ptr)[(unsigned)(c)]&(_P|_U|_L|_N))
#define iscntrl(c)	((__ctype_ptr)[(unsigned)(c)]&_C)

# define toupper(c) \
	__extension__ ({ int __x = (c); islower(__x) ? (__x - 'a' + 'A') : __x;})
# define tolower(c) \
	__extension__ ({ int __x = (c); isupper(__x) ? (__x - 'A' + 'a') : __x;})
#endif /* !__cplusplus */

#ifndef __STRICT_ANSI__
#define isascii(c)	((unsigned)(c)<=0177)
#define toascii(c)	((c)&0177)
#endif

_END_STD_C

#endif
