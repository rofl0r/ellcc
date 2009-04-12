#ifndef _WCHAR_H_
#define _WCHAR_H_

#include <_ansi.h>
_BEGIN_STD_C

#include <sys/reent.h>

#define __need_size_t
#define __need_wchar_t
#define __need_wint_t
#include <stddef.h>

/* For _mbstate_t definition. */
#include <sys/_types.h>

#ifndef NULL
#define NULL	0
#endif

#ifndef WEOF
# define WEOF ((wint_t)-1)
#endif

#ifndef WCHAR_MIN
#define WCHAR_MIN 0
#endif

#ifndef WCHAR_MAX
#ifdef __WCHAR_MAX__
#define WCHAR_MAX __WCHAR_MAX__
#else
#define WCHAR_MAX 0x7fffffffu
#endif
#endif

#ifndef _MBSTATE_T
#define _MBSTATE_T
typedef _mbstate_t mbstate_t;
#endif /* _MBSTATE_T */

wint_t	btowc(int);
int	wctob(wint_t);
size_t	mbrlen(const char * , size_t, mbstate_t *);
size_t	mbrtowc(wchar_t * , const char * , size_t, mbstate_t *);
size_t	_mbrtowc_r(struct _reent *, wchar_t * , const char * , 
			size_t, mbstate_t *);
int	mbsinit(const mbstate_t *);
size_t	mbsrtowcs(wchar_t * , const char ** , size_t, mbstate_t *);
size_t	wcrtomb(char * , wchar_t, mbstate_t *);
size_t	_wcrtomb_r(struct _reent *, char * , wchar_t, mbstate_t *);
size_t	wcsrtombs(char * , const wchar_t ** , size_t, mbstate_t *);
size_t	_wcsrtombs_r(struct _reent *, char * , const wchar_t ** , 
			size_t, mbstate_t *);
wchar_t	*wcscat(wchar_t * , const wchar_t *);
wchar_t	*wcschr(const wchar_t *, wchar_t);
int	wcscmp(const wchar_t *, const wchar_t *);
int	wcscoll(const wchar_t *, const wchar_t *);
wchar_t	*wcscpy(wchar_t * , const wchar_t *);
wchar_t	*wcpcpy(wchar_t * , const wchar_t *);
size_t	wcscspn(const wchar_t *, const wchar_t *);
size_t	wcslcat(wchar_t *, const wchar_t *, size_t);
size_t	wcslcpy(wchar_t *, const wchar_t *, size_t);
size_t	wcslen(const wchar_t *);
wchar_t	*wcsncat(wchar_t * , const wchar_t * , size_t);
int	wcsncmp(const wchar_t *, const wchar_t *, size_t);
wchar_t	*wcsncpy(wchar_t *  , const wchar_t * , size_t);
wchar_t	*wcpncpy(wchar_t *  , const wchar_t * , size_t);
size_t	wcsnlen(const wchar_t *, size_t);
wchar_t	*wcspbrk(const wchar_t *, const wchar_t *);
wchar_t	*wcsrchr(const wchar_t *, wchar_t);
size_t	wcsspn(const wchar_t *, const wchar_t *);
wchar_t	*wcsstr(const wchar_t *, const wchar_t *);
int	wcswidth(const wchar_t *, size_t);
size_t	wcsxfrm(wchar_t *, const wchar_t *, size_t);
int	wcwidth(const wchar_t);
wchar_t	*wmemchr(const wchar_t *, wchar_t, size_t);
int	wmemcmp(const wchar_t *, const wchar_t *, size_t);
wchar_t	*wmemcpy(wchar_t * , const wchar_t * , size_t);
wchar_t	*wmemmove(wchar_t *, const wchar_t *, size_t);
wchar_t	*wmemset(wchar_t *, wchar_t, size_t);

long    wcstol(const wchar_t *, wchar_t **, int);
long long wcstoll(const wchar_t *, wchar_t **, int);
unsigned long wcstoul(const wchar_t *, wchar_t **, int);
unsigned long long wcstoull(const wchar_t *, wchar_t **, int);
long    _wcstol_r(struct _reent *, const wchar_t *, wchar_t **, int);
long long _wcstoll_r(struct _reent *, const wchar_t *, wchar_t **, int);
unsigned long _wcstoul_r(struct _reent *, const wchar_t *, wchar_t **, int);
unsigned long long _wcstoull_r(struct _reent *, const wchar_t *, wchar_t **, int);

_END_STD_C

#endif /* _WCHAR_H_ */
