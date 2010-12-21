#ifndef  _MATH_H_
#define  _MATH_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <sys/reent.h>
#include <ieeefp.h>

union __dmath
{
  __ULong i[2];
  double d;
};

union __fmath
{
  __ULong i[1];
  float f;
};

union __ldmath
{
  __ULong i[4];
  long double ld;
};

/* Natural log of 2 */
#define _M_LOG2_E        0.693147180559945309417

#if defined(__GNUC__) && \
  ( (__GNUC__ >= 4) || \
    ( (__GNUC__ >= 3) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 3) ) )

 /* gcc >= 3.3 implicitly defines builtins for HUGE_VALx values.  */

# ifndef HUGE_VAL
#  define HUGE_VAL (__builtin_huge_val())
# endif

# ifndef HUGE_VALF
#  define HUGE_VALF (__builtin_huge_valf())
# endif

# ifndef HUGE_VALL
#  define HUGE_VALL (__builtin_huge_vall())
# endif

# ifndef INFINITY
#  define INFINITY (__builtin_inff())
# endif

# ifndef NAN
#  define NAN (__builtin_nanf(""))
# endif

#else /* !gcc >= 3.3  */

 /* No builtins.  Use floating-point unions instead.  Declare as an array
    without bounds so no matter what small data support a port and/or
    library has, the reference will be via the general method for accessing
    globals. */

 #ifndef HUGE_VAL
  extern gconst union __dmath __infinity[];
  #define HUGE_VAL (__infinity[0].d)
 #endif

 #ifndef HUGE_VALF
  extern gconst union __fmath __infinityf[];
  #define HUGE_VALF (__infinityf[0].f)
 #endif

 #ifndef HUGE_VALL
  extern gconst union __ldmath __infinityld[];
  #define HUGE_VALL (__infinityld[0].ld)
 #endif

#endif /* !gcc >= 3.3  */

/* Reentrant ANSI C functions.  */

extern double atan(double);
extern double cos(double);
extern double sin(double);
extern double tan(double);
extern double tanh(double);
extern double frexp(double, int *);
extern double modf(double, double *);
extern double ceil(double);
extern double fabs(double);
extern double floor(double);
extern double logb(double);

/* Non reentrant ANSI C functions.  */

#ifndef _REENT_ONLY
extern double acos(double);
extern double asin(double);
extern double atan2(double, double);
extern double cosh(double);
extern double sinh(double);
extern double exp(double);
extern double ldexp(double, int);
extern double log(double);
extern double log10(double);
extern double pow(double, double);
extern double sqrt(double);
extern double fmod(double, double);
#endif /* ! defined (_REENT_ONLY) */

#if !defined(__STRICT_ANSI__) || defined(__cplusplus) || __STDC_VERSION__ >= 199901L

/* ISO C99 types and macros. */

#ifndef FLT_EVAL_METHOD
#define FLT_EVAL_METHOD 0
typedef float float_t;
typedef double double_t;
#endif /* FLT_EVAL_METHOD */

#define FP_NAN         0
#define FP_INFINITE    1
#define FP_ZERO        2
#define FP_SUBNORMAL   3
#define FP_NORMAL      4

#ifndef FP_ILOGB0
# define FP_ILOGB0 (-INT_MAX)
#endif
#ifndef FP_ILOGBNAN
# define FP_ILOGBNAN INT_MAX
#endif

#ifndef MATH_ERRNO
# define MATH_ERRNO 1
#endif
#ifndef MATH_ERREXCEPT
# define MATH_ERREXCEPT 2
#endif
#ifndef math_errhandling
# define math_errhandling MATH_ERRNO
#endif

#if RICH
extern int __isinff(float x);
extern int __isinfd(double x);
extern int __isnanf(float x);
extern int __isnand(double x);
#endif
extern int __fpclassifyf(float x);
extern int __fpclassifyd(double x);
extern int __signbitf(float x);
extern int __signbitd(double x);

#define fpclassify(x) \
          (__extension__ ({__typeof__(x) __x = (x); \
                           (sizeof (__x) == sizeof (float))  ? __fpclassifyf(__x) : __fpclassifyd(__x);}))

#ifndef isfinite
#define isfinite(y) \
          (__extension__ ({__typeof__(y) __y = (y); \
                           fpclassify(__y) != FP_INFINITE && fpclassify(__y) != FP_NAN;}))
#endif

#ifndef isinf
#define isinf(x) \
          (__extension__ ({__typeof__(x) __x = (x); \
                           (sizeof (__x) == sizeof (float))  ? __isinff(__x) : __isinfd(__x);}))
#endif

#ifndef isnan
#define isnan(x) \
          (__extension__ ({__typeof__(x) __x = (x); \
                           (sizeof (__x) == sizeof (float))  ? __isnanf(__x) : __isnand(__x);}))
#endif

#define isnormal(y) (fpclassify(y) == FP_NORMAL)
#define signbit(x) \
          (__extension__ ({__typeof__(x) __x = (x); \
                           (sizeof(__x) == sizeof(float)) ? __signbitf(__x) : __signbitd(__x);}))

#define isgreater(x,y) \
          (__extension__ ({__typeof__(x) __x = (x); __typeof__(y) __y = (y); \
                           !isunordered(__x,__y) && (__x > __y);}))
#define isgreaterequal(x,y) \
          (__extension__ ({__typeof__(x) __x = (x); __typeof__(y) __y = (y); \
                           !isunordered(__x,__y) && (__x >= __y);}))
#define isless(x,y) \
          (__extension__ ({__typeof__(x) __x = (x); __typeof__(y) __y = (y); \
                           !isunordered(__x,__y) && (__x < __y);}))
#define islessequal(x,y) \
          (__extension__ ({__typeof__(x) __x = (x); __typeof__(y) __y = (y); \
                           !isunordered(__x,__y) && (__x <= __y);}))
#define islessgreater(x,y) \
          (__extension__ ({__typeof__(x) __x = (x); __typeof__(y) __y = (y); \
                           !isunordered(__x,__y) && (__x < __y || __x > __y);}))

#define isunordered(a,b) \
          (__extension__ ({__typeof__(a) __a = (a); __typeof__(b) __b = (b); \
                           fpclassify(__a) == FP_NAN || fpclassify(__b) == FP_NAN;}))

/* Non ANSI double precision functions.  */

extern double infinity(void);
extern double nan(const char *);
extern int finite(double);
extern double copysign(double, double);
extern int ilogb(double);

extern double asinh(double);
extern double cbrt(double);
extern double nextafter(double, double);
extern double rint(double);
extern double scalbn(double, int);

extern double exp2(double);
extern double scalbln(double, long int);
extern double tgamma(double);
extern double nearbyint(double);
extern long int lrint(double);
extern long long int llrint(double);
extern double round(double);
extern long int lround(double);
extern double trunc(double);
extern double remquo(double, double, int *);
extern double copysign(double, double);
extern double fdim(double, double);
extern double fmax(double, double);
extern double fmin(double, double);
extern double fma(double, double, double);

extern double log1p(double);
extern double expm1(double);

#ifndef _REENT_ONLY
extern double acosh(double);
extern double atanh(double);
extern double remainder(double, double);
extern double gamma (double);
extern double lgamma (double);
extern double erf (double);
extern double erfc (double);
#define log2(x) (log (x) / _M_LOG2_E)

extern double hypot(double, double);

#endif /* ! defined (_REENT_ONLY) */

/* Single precision versions of ANSI functions.  */
extern float atanf(float);
extern float cosf(float);
extern float sinf(float);
extern float tanf(float);
extern float tanhf(float);
extern float frexpf(float, int *);
extern float modff(float, float *);
extern float ceilf(float);
extern float fabsf(float);
extern float floorf(float);
extern float logbf(float);

#ifndef _REENT_ONLY
extern float acosf(float);
extern float asinf(float);
extern float atan2f(float, float);
extern float coshf(float);
extern float sinhf(float);
extern float expf(float);
extern float ldexpf(float, int);
extern float logf(float);
extern float log10f(float);
extern float powf(float, float);
extern float sqrtf(float);
extern float fmodf(float, float);
#endif /* ! defined (_REENT_ONLY) */

/* Other single precision functions.  */

extern float exp2f(float);
extern float scalblnf(float, long int);
extern float tgammaf(float);
extern float nearbyintf(float);
extern long int lrintf(float);
extern long long llrintf(float);
extern float roundf(float);
extern long int lroundf(float);
extern float truncf(float);
extern float remquof(float, float, int *);
extern float copysignf(float, float);
extern float fdimf(float, float);
extern float fmaxf(float, float);
extern float fminf(float, float);
extern float fmaf(float, float, float);

extern float infinityf(void);
extern float nanf(const char *);
#if RICH
#ifndef isnanf
extern int isnanf(float);
#endif
#ifndef isinff
extern int isinff(float);
#endif
#ifndef finitef
extern int finitef(float);
#endif
#endif
extern float copysignf(float, float);
extern int ilogbf(float);

extern float asinhf(float);
extern float cbrtf(float);
extern float nextafterf(float, float);
extern float rintf(float);
extern float scalbnf(float, int);
extern float log1pf(float);
extern float expm1f(float);

#ifndef _REENT_ONLY
extern float acoshf(float);
extern float atanhf(float);
extern float remainderf(float, float);
extern float gammaf(float);
extern float lgammaf(float);
extern float erff(float);
extern float erfcf(float);
#define log2f(x) (logf (x) / (float) _M_LOG2_E)
extern float hypotf (float, float);
#endif /* ! defined (_REENT_ONLY) */

/* Other long double precision functions.  */
extern long double rintl(long double);
extern long int lrintl(long double);
extern long long llrintl(long double);
extern long double logbl(long double);

#endif /* !defined (__STRICT_ANSI__) || defined(__cplusplus) || __STDC_VERSION__ >= 199901L */

#if !defined (__STRICT_ANSI__) || defined(__cplusplus)

extern double cabs();
extern double drem(double, double);
extern void sincos(double, double *, double *);
extern double gamma_r(double, int *);
extern double lgamma_r(double, int *);

extern double y0(double);
extern double y1(double);
extern double yn(int, double);
extern double j0(double);
extern double j1(double);
extern double jn(int, double);

extern float cabsf();
extern float dremf(float, float);
extern void sincosf(float, float *, float *);
extern float gammaf_r(float, int *);
extern float lgammaf_r(float, int *);

extern float y0f(float);
extern float y1f(float);
extern float ynf(int, float);
extern float j0f(float);
extern float j1f(float);
extern float jnf(int, float);

/* GNU extensions */
# ifndef exp10
extern double exp10(double);
# endif
# ifndef pow10
extern double pow10(double);
# endif
# ifndef exp10f
extern float exp10f(float);
# endif
# ifndef pow10f
extern float pow10f(float);
# endif

#endif /* !defined (__STRICT_ANSI__) || defined(__cplusplus) */

#ifndef __STRICT_ANSI__

/* The gamma functions use a global variable, signgam.  */
#ifndef _REENT_ONLY
#define signgam (*__signgam())
extern int *__signgam(void);
#endif /* ! defined (_REENT_ONLY) */

#define __signgam_r(ptr) _REENT_SIGNGAM(ptr)

/* The exception structure passed to the matherr routine.  */
/* We have a problem when using C++ since `exception' is a reserved
   name in C++.  */
#ifdef __cplusplus
struct __exception
#else
struct exception
#endif
{
  int type;
  char *name;
  double arg1;
  double arg2;
  double retval;
  int err;
};

#ifdef __cplusplus
extern int matherr(struct __exception *e);
#else
extern int matherr(struct exception *e);
#endif

/* Values for the type field of struct exception.  */

#define DOMAIN 1
#define SING 2
#define OVERFLOW 3
#define UNDERFLOW 4
#define TLOSS 5
#define PLOSS 6

/* Useful constants.  */

#define MAXFLOAT	3.40282347e+38F

#define M_E		2.7182818284590452354
#define M_LOG2E		1.4426950408889634074
#define M_LOG10E	0.43429448190325182765
#define M_LN2		0.69314718055994530942
#define M_LN10		2.30258509299404568402
#define M_PI		3.14159265358979323846
#define M_TWOPI         (M_PI * 2.0)
#define M_PI_2		1.57079632679489661923
#define M_PI_4		0.78539816339744830962
#define M_3PI_4		2.3561944901923448370E0
#define M_SQRTPI        1.77245385090551602792981
#define M_1_PI		0.31830988618379067154
#define M_2_PI		0.63661977236758134308
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2		1.41421356237309504880
#define M_SQRT1_2	0.70710678118654752440
#define M_LN2LO         1.9082149292705877000E-10
#define M_LN2HI         6.9314718036912381649E-1
#define M_SQRT3	        1.73205080756887719000
#define M_IVLN10        0.43429448190325182765 /* 1 / log(10) */
#define M_LOG2_E        _M_LOG2_E
#define M_INVLN2        1.4426950408889633870E0  /* 1 / log(2) */

/* Global control over fdlibm error handling.  */

enum __fdlibm_version
{
  __fdlibm_ieee = -1,
  __fdlibm_svid,
  __fdlibm_xopen,
  __fdlibm_posix
};

#define _LIB_VERSION_TYPE enum __fdlibm_version
#define _LIB_VERSION __fdlib_version

extern _LIB_VERSION_TYPE _LIB_VERSION;

#define _IEEE_  __fdlibm_ieee
#define _SVID_  __fdlibm_svid
#define _XOPEN_ __fdlibm_xopen
#define _POSIX_ __fdlibm_posix

#endif /* ! defined (__STRICT_ANSI__) */

#ifdef __cplusplus
}
#endif

#ifdef __FAST_MATH__
#include <machine/fastmath.h>
#endif

#endif /* _MATH_H_ */
