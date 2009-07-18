/* assert.h
 */

#include "_ansi.h"

_BEGIN_STD_C

#undef assert

#ifdef NDEBUG           /* required by ANSI standard */
# define assert(__e) ((void)0)
#else
# define assert(__e) ((__e) ? (void)0 : __assert_func (__FILE__, __LINE__, \
						       __ASSERT_FUNC, #__e))

# ifndef __ASSERT_FUNC
  /* Use g++'s demangled names in C++.  */
#  if defined __cplusplus && defined __GNUC__
#   define __ASSERT_FUNC __PRETTY_FUNCTION__

  /* C99 requires the use of __func__, gcc also supports it.  */
#  elif defined __GNUC__ || __STDC_VERSION__ >= 199901L
#   define __ASSERT_FUNC __func__

  /* failed to detect __func__ support.  */
#  else
#   define __ASSERT_FUNC ((char *) 0)
#  endif
# endif /* !__ASSERT_FUNC */
#endif /* !NDEBUG */

void __assert(const char *, int, const char *) __attribute__ ((__noreturn__));
void __assert_func(const char *, int, const char *, const char *) __attribute__ ((__noreturn__));

_END_STD_C
