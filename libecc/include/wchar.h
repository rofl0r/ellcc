/** Wide character definitions.
 *      wchar_t         wint_t          WCHAR_MAX
 *      size_t          struct tm       WCHAR_MIN
 *      mbstate_t       NULL            WEOF
 */

#ifndef _WCHAR_H_
#define _WCHAR_H_

_BEGIN_STD_C

#include <sys/reent.h>
#include <sys/_types.h> // _mbstate_t

/** A type to represent a wide character.
 * __WCHAR_TYPE__ is defined by the compiler.
 */
typedef __WCHAR_TYPE__ wchar_t;
/** A type to represent a wide character as a parameter.
 * __WINT_TYPE__ is defined by the compiler.
 */
typedef __WINT_TYPE__ wint_t;
/** A type to represent the size of an object.
 * __SIZE_TYPE__ is defined by the compiler.
 */
typedef __SIZE_TYPE__ size_t;                   
/** A pointer to nothing.
 */
#define NULL 0
/** A wide character representing end of file.
 */
#define WEOF ((wint_t)-1)
/** The minimum value of a wchar_t.
 * __WCHAR_MIN__ is defined by the compiler.
 */
#define WCHAR_MIN __WCHAR_MIN__
/** The maximum value of a wchar.
 * __WCHAR_MAX__ is defined by the compiler.
 */
#define WCHAR_MAX __WCHAR_MAX__

#ifndef _MBSTATE_T
#define _MBSTATE_T
typedef _mbstate_t mbstate_t;
#endif /* _MBSTATE_T */

wint_t btowc(int);
int wctob(wint_t);
size_t mbrlen(const char * , size_t, mbstate_t *);
size_t mbrtowc(wchar_t * , const char * , size_t, mbstate_t *);
size_t _mbrtowc_r(struct _reent *, wchar_t * , const char * , size_t, mbstate_t *);
int mbsinit(const mbstate_t *);
size_t mbsrtowcs(wchar_t * , const char ** , size_t, mbstate_t *);
size_t wcrtomb(char * , wchar_t, mbstate_t *);
size_t _wcrtomb_r(struct _reent *, char * , wchar_t, mbstate_t *);
size_t wcsrtombs(char * , const wchar_t ** , size_t, mbstate_t *);
size_t _wcsrtombs_r(struct _reent *, char * , const wchar_t ** , size_t, mbstate_t *);
wchar_t *wcscat(wchar_t * , const wchar_t *);
wchar_t *wcschr(const wchar_t *, wchar_t);
int wcscmp(const wchar_t *, const wchar_t *);
int wcscoll(const wchar_t *, const wchar_t *);
wchar_t *wcscpy(wchar_t * , const wchar_t *);
wchar_t *wcpcpy(wchar_t * , const wchar_t *);
size_t wcscspn(const wchar_t *, const wchar_t *);
size_t wcslcat(wchar_t *, const wchar_t *, size_t);
size_t wcslcpy(wchar_t *, const wchar_t *, size_t);
size_t wcslen(const wchar_t *);
wchar_t *wcsncat(wchar_t * , const wchar_t * , size_t);
int wcsncmp(const wchar_t *, const wchar_t *, size_t);
wchar_t *wcsncpy(wchar_t *  , const wchar_t * , size_t);
wchar_t *wcpncpy(wchar_t *  , const wchar_t * , size_t);
size_t wcsnlen(const wchar_t *, size_t);
wchar_t *wcspbrk(const wchar_t *, const wchar_t *);
wchar_t *wcsrchr(const wchar_t *, wchar_t);
size_t wcsspn(const wchar_t *, const wchar_t *);
wchar_t *wcsstr(const wchar_t *, const wchar_t *);
int wcswidth(const wchar_t *, size_t);
size_t wcsxfrm(wchar_t *, const wchar_t *, size_t);
int wcwidth(const wchar_t);
wchar_t *wmemchr(const wchar_t *, wchar_t, size_t);
int wmemcmp(const wchar_t *, const wchar_t *, size_t);
wchar_t *wmemcpy(wchar_t * , const wchar_t * , size_t);
wchar_t *wmemmove(wchar_t *, const wchar_t *, size_t);
wchar_t *wmemset(wchar_t *, wchar_t, size_t);

long wcstol(const wchar_t *, wchar_t **, int);
long long wcstoll(const wchar_t *, wchar_t **, int);
unsigned long wcstoul(const wchar_t *, wchar_t **, int);
unsigned long long wcstoull(const wchar_t *, wchar_t **, int);
long _wcstol_r(struct _reent *, const wchar_t *, wchar_t **, int);
long long _wcstoll_r(struct _reent *, const wchar_t *, wchar_t **, int);
unsigned long _wcstoul_r(struct _reent *, const wchar_t *, wchar_t **, int);
unsigned long long _wcstoull_r(struct _reent *, const wchar_t *, wchar_t **, int);

_END_STD_C

#endif /* _WCHAR_H_ */
