/** @file
 * Definitions for common types, variables, and functions.
 */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
// #include <sys/reent.h>
// #include <machine/stdlib.h>
#ifndef __STRICT_ANSI__
// #include <alloca.h>
#endif

/** The return type of @ref div().
 */
typedef struct 
{
    int quot;           ///< The quotient.
    int rem;            ///< The remainder.
} div_t;

/** The return type of @ref ldiv().
 */
typedef struct 
{
    long quot;          ///< The quotient.
    long rem;           ///< The remainder.
} ldiv_t;

/** The return type of @ref lldiv().
 */
typedef struct
{
    long long quot;     ///< The quotient.
    long long rem;      ///< The remainder.
} lldiv_t;

#ifndef NULL
/** A null pointer.
 */
#define NULL 0
#endif

/** A failure @ref exit() value.
 */
#define EXIT_FAILURE 1

/** The success @ref exit() value.
 */
#define EXIT_SUCCESS 0

/** The maximum value returned by @ref rand().
 */
#define RAND_MAX __INT_MAX__

/** The maximum length of a multibyte character.
 */
extern int __mb_cur_max;

/** The maximum length of a multibyte character.
 */
#define MB_CUR_MAX __mb_cur_max

/** Abort the calling process.
 */
void abort(void) __attribute__ ((noreturn));

/** Return the absolute value of the argument.
 * @param arg The agrument.
 * @return The absolute value.
 */
int abs(int arg);

/** Register a function to be called at program exit.
 *
 * The atexit() function is called either as a result of a call
 * to the function @ref exit() or because the program's main()
 * returns.
 *
 * @param func The function to be called.
 * @return 0 if successful, otherwise a non-zero value.
 */
int atexit(void (*__func)(void));

/** Convert a string to a double.
 * @param nptr The string to convert.
 * @return The converted string.
 */
double atof(const char *nptr);

/** Convert a string to an int.
 * @param nptr The string to convert.
 * @return The converted string.
 */
int atoi(const char *nptr);

/** Convert a string to a long.
 * @param nptr The string to convert.
 * @return The converted string.
 */
long atol(const char *nptr);

/** Search a sorted array.
 *  The array is assumed to be sorted in ascending order based
 *  on the compar() function. The compare() function should return
 *  a value less than zero if the first argument (the key) is less
 *  than the second, equal to zero if the values are the same, and
 *  greater than zero if the first argument is greater than the second.
 *
 *  @param key The value to find.
 *  @param base The base of the array of values.
 *  @param nmemb The number of objects in the array.
 *  @param size The size of each member.
 *  @param compar The comparison function described above.
 *  @return The matching array entry or NULL if none is found.
 */ 
void *bsearch(const void *key, const void *base,
              size_t nmemb, size_t size,
              int (*compar)(const void *, const void *));

/** Allocate memory for an array.
 * This function allocates memory for an array and returns a
 * pointer to the allocated array that has been filled with zeros.
 *
 * @param nmemb The number of elements in the array.
 * @param size The size of each element.
 * @return a pointer to the array, or NULL if space isn't available.
 */
void *calloc(size_t nmemb, size_t size);

div_t div(int __numer, int __denom);
void exit(int __status) __attribute__ ((noreturn));
void free(void *);
char * getenv(const char *__string);
char * _getenv_r(struct _reent *, const char *__string);
char * _findenv(const char *, int *);
char * _findenv_r(struct _reent *, const char *, int *);
long labs(long);
ldiv_t ldiv(long __numer, long __denom);
void * malloc(size_t __size);
int mblen(const char *, size_t);
int mbtowc(wchar_t *, const char *, size_t);
int wctomb(char *, wchar_t);
size_t mbstowcs(wchar_t *, const char *, size_t);
size_t wcstombs(char *, const wchar_t *, size_t);
#ifndef __STRICT_ANSI__
#ifndef _REENT_ONLY
int mkstemp(char *);
char * mktemp(char *);
#endif
#endif
void qsort(void * __base, size_t __nmemb, size_t __size, int(*_compar)(const void *, const void *));
int rand(void);
void * realloc(void * __r, size_t __size);
void srand(unsigned __seed);
double strtod(const char *__n, char **__end_PTR);
double _strtod_r(struct _reent *,const char *__n, char **__end_PTR);
float strtof(const char *__n, char **__end_PTR);
#ifndef __STRICT_ANSI__
/* the following strtodf interface is deprecated...use strtof instead */
# ifndef strtodf 
# define strtodf strtof
# endif
#endif
long strtol(const char *__n, char **__end_PTR, int __base);
long _strtol_r(struct _reent *,const char *__n, char **__end_PTR, int __base);
unsigned long strtoul(const char *__n, char **__end_PTR, int __base);
unsigned long _strtoul_r(struct _reent *,const char *__n, char **__end_PTR, int __base);

int system(const char *__string);

#ifndef __STRICT_ANSI__
long a64l(const char *__input);
char * l64a(long __input);
char * _l64a_r(struct _reent *,long __input);
int on_exit(void (*__func)(int, void *),void * __arg);
void _Exit(int __status) __attribute__ ((noreturn));
int putenv(char *__string);
int _putenv_r(struct _reent *, char *__string);
int setenv(const char *__string, const char *__value, int __overwrite);
int _setenv_r(struct _reent *, const char *__string, const char *__value, int __overwrite);

char * gcvt(double,int,char *);
char * gcvtf(float,int,char *);
char * fcvt(double,int,int *,int *);
char * fcvtf(float,int,int *,int *);
char * ecvt(double,int,int *,int *);
char * ecvtbuf(double, int, int*, int*, char *);
char * fcvtbuf(double, int, int*, int*, char *);
char * ecvtf(float,int,int *,int *);
char * dtoa(double, int, int, int *, int*, char**);
int rand_r(unsigned *__seed);

double drand48(void);
double _drand48_r(struct _reent *);
double erand48(unsigned short [3]);
double _erand48_r(struct _reent *, unsigned short [3]);
long jrand48(unsigned short [3]);
long _jrand48_r(struct _reent *, unsigned short [3]);
void lcong48(unsigned short [7]);
void _lcong48_r(struct _reent *, unsigned short [7]);
long lrand48(void);
long _lrand48_r(struct _reent *);
long mrand48(void);
long _mrand48_r(struct _reent *);
long nrand48(unsigned short [3]);
long _nrand48_r(struct _reent *, unsigned short [3]);
unsigned short *
 seed48(unsigned short [3]);
unsigned short *
 _seed48_r(struct _reent *, unsigned short [3]);
void srand48(long);
void _srand48_r(struct _reent *, long);
long long atoll(const char *__nptr);
long long _atoll_r(struct _reent *, const char *__nptr);
long long llabs(long long);
lldiv_t lldiv(long long __numer, long long __denom);
long long strtoll(const char *__n, char **__end_PTR, int __base);
long long _strtoll_r(struct _reent *, const char *__n, char **__end_PTR, int __base);
unsigned long long strtoull(const char *__n, char **__end_PTR, int __base);
unsigned long long _strtoull_r(struct _reent *, const char *__n, char **__end_PTR, int __base);

#ifndef __CYGWIN__
void cfree(void *);
void unsetenv(const char *__string);
void _unsetenv_r(struct _reent *, const char *__string);
#endif

#endif /* ! __STRICT_ANSI__ */

char * _dtoa_r(struct _reent *, double, int, int, int *, int*, char**);
#ifndef __CYGWIN__
void * _malloc_r(struct _reent *, size_t);
void * _calloc_r(struct _reent *, size_t, size_t);
void _free_r(struct _reent *, void *);
void * _realloc_r(struct _reent *, void *, size_t);
void _mstats_r(struct _reent *, char *);
#endif
int _system_r(struct _reent *, const char *);

void __eprintf(const char *, const char *, unsigned int, const char *);

#ifdef __cplusplus
}
#endif

#endif
