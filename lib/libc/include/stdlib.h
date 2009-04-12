/*
 * stdlib.h
 *
 * Definitions for common types, variables, and functions.
 */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#include "_ansi.h"
_END_STD_C

#define __need_size_t
#define __need_wchar_t
#include <stddef.h>

#include <sys/reent.h>
#include <machine/stdlib.h>
#ifndef __STRICT_ANSI__
#include <alloca.h>
#endif

typedef struct 
{
  int quot; /* quotient */
  int rem; /* remainder */
} div_t;

typedef struct 
{
  long quot; /* quotient */
  long rem; /* remainder */
} ldiv_t;

#ifndef __STRICT_ANSI__
typedef struct
{
  long long int quot; /* quotient */
  long long int rem; /* remainder */
} lldiv_t;
#endif

#ifndef NULL
#define NULL 0
#endif

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define RAND_MAX __RAND_MAX

extern int __mb_cur_max;

#define MB_CUR_MAX __mb_cur_max

void	abort(void) __attribute__ ((noreturn));
int	abs(int);
int	atexit(void (*__func)(void));
double	atof(const char *__nptr);
#ifndef __STRICT_ANSI__
float	atoff(const char *__nptr);
#endif
int	atoi(const char *__nptr);
int	_atoi_r(struct _reent *, const char *__nptr);
long	atol(const char *__nptr);
long	_atol_r(struct _reent *, const char *__nptr);
void *	bsearch(const void * __key,
		       const void * __base,
		       size_t __nmemb,
		       size_t __size,
		       int (*_compar)(const void *, const void *));
void *	calloc(size_t __nmemb, size_t __size);
div_t	div(int __numer, int __denom);
void	exit(int __status) __attribute__ ((noreturn));
void	free(void *);
char *  getenv(const char *__string);
char *	_getenv_r(struct _reent *, const char *__string);
char *	_findenv(const char *, int *);
char *	_findenv_r(struct _reent *, const char *, int *);
long	labs(long);
ldiv_t	ldiv(long __numer, long __denom);
void *	malloc(size_t __size);
int	mblen(const char *, size_t);
int	_mblen_r(struct _reent *, const char *, size_t, _mbstate_t *);
int	mbtowc(wchar_t *, const char *, size_t);
int	_mbtowc_r(struct _reent *, wchar_t *, const char *, size_t, _mbstate_t *);
int	wctomb(char *, wchar_t);
int	_wctomb_r(struct _reent *, char *, wchar_t, _mbstate_t *);
size_t	mbstowcs(wchar_t *, const char *, size_t);
size_t	_mbstowcs_r(struct _reent *, wchar_t *, const char *, size_t, _mbstate_t *);
size_t	wcstombs(char *, const wchar_t *, size_t);
size_t	_wcstombs_r(struct _reent *, char *, const wchar_t *, size_t, _mbstate_t *);
#ifndef __STRICT_ANSI__
#ifndef _REENT_ONLY
int     mkstemp(char *);
char *  mktemp(char *);
#endif
#endif
void	qsort(void * __base, size_t __nmemb, size_t __size, int(*_compar)(const void *, const void *));
int	rand(void);
void *	realloc(void * __r, size_t __size);
void	srand(unsigned __seed);
double	strtod(const char *__n, char **__end_PTR);
double	_strtod_r(struct _reent *,const char *__n, char **__end_PTR);
float	strtof(const char *__n, char **__end_PTR);
#ifndef __STRICT_ANSI__
/* the following strtodf interface is deprecated...use strtof instead */
# ifndef strtodf 
#  define strtodf strtof
# endif
#endif
long	strtol(const char *__n, char **__end_PTR, int __base);
long	_strtol_r(struct _reent *,const char *__n, char **__end_PTR, int __base);
unsigned long strtoul(const char *__n, char **__end_PTR, int __base);
unsigned long _strtoul_r(struct _reent *,const char *__n, char **__end_PTR, int __base);

int	system(const char *__string);

#ifndef __STRICT_ANSI__
long    a64l(const char *__input);
char *  l64a(long __input);
char *  _l64a_r(struct _reent *,long __input);
int	on_exit(void (*__func)(int, void *),void * __arg);
void	_Exit(int __status) __attribute__ ((noreturn));
int	putenv(char *__string);
int	_putenv_r(struct _reent *, char *__string);
int	setenv(const char *__string, const char *__value, int __overwrite);
int	_setenv_r(struct _reent *, const char *__string, const char *__value, int __overwrite);

char *	gcvt(double,int,char *);
char *	gcvtf(float,int,char *);
char *	fcvt(double,int,int *,int *);
char *	fcvtf(float,int,int *,int *);
char *	ecvt(double,int,int *,int *);
char *	ecvtbuf(double, int, int*, int*, char *);
char *	fcvtbuf(double, int, int*, int*, char *);
char *	ecvtf(float,int,int *,int *);
char *	dtoa(double, int, int, int *, int*, char**);
int	rand_r(unsigned *__seed);

double drand48(void);
double _drand48_r(struct _reent *);
double erand48(unsigned short [3]);
double _erand48_r(struct _reent *, unsigned short [3]);
long   jrand48(unsigned short [3]);
long   _jrand48_r(struct _reent *, unsigned short [3]);
void  lcong48(unsigned short [7]);
void  _lcong48_r(struct _reent *, unsigned short [7]);
long   lrand48(void);
long   _lrand48_r(struct _reent *);
long   mrand48(void);
long   _mrand48_r(struct _reent *);
long   nrand48(unsigned short [3]);
long   _nrand48_r(struct _reent *, unsigned short [3]);
unsigned short *
       seed48(unsigned short [3]);
unsigned short *
       _seed48_r(struct _reent *, unsigned short [3]);
void  srand48(long);
void  _srand48_r(struct _reent *, long);
long long atoll(const char *__nptr);
long long _atoll_r(struct _reent *, const char *__nptr);
long long llabs(long long);
lldiv_t	lldiv(long long __numer, long long __denom);
long long strtoll(const char *__n, char **__end_PTR, int __base);
long long _strtoll_r(struct _reent *, const char *__n, char **__end_PTR, int __base);
unsigned long long strtoull(const char *__n, char **__end_PTR, int __base);
unsigned long long _strtoull_r(struct _reent *, const char *__n, char **__end_PTR, int __base);

#ifndef __CYGWIN__
void	cfree(void *);
void	unsetenv(const char *__string);
void	_unsetenv_r(struct _reent *, const char *__string);
#endif

#endif /* ! __STRICT_ANSI__ */

char *	_dtoa_r(struct _reent *, double, int, int, int *, int*, char**);
#ifndef __CYGWIN__
void *	_malloc_r(struct _reent *, size_t);
void *	_calloc_r(struct _reent *, size_t, size_t);
void	_free_r(struct _reent *, void *);
void *	_realloc_r(struct _reent *, void *, size_t);
void	_mstats_r(struct _reent *, char *);
#endif
int	_system_r(struct _reent *, const char *);

void	__eprintf(const char *, const char *, unsigned int, const char *);

_END_STD_C

#endif /* _STDLIB_H_ */
