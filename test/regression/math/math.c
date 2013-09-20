// Compile and run for every target.
// RUN: %ecc -o %t %s -lm && %t
// RUN: %armecc -o %t %s -lm && %armrun %t
// RUN: %armebecc -o %t %s -lm && %armebrun %t
// RUN: %i386ecc -o %t %s -lm && %i386run %t
// RUN: %microblazeecc -o %t %s -lm && %microblazerun %t
// RUN: %mipsecc -o %t %s -lm && %mipsrun %t
// RUN: %mipselecc -o %t %s -lm && %mipselrun %t
// RUN: %ppcecc -o %t %s -lm && %ppcrun %t
// FAIL: %ppc64ecc -o %t %s -lm && %ppc64run %t
// RUN: %x86_64ecc -o %t %s -lm && %x86_64run %t
#include "../ecc_test.h"
#include <math.h>
#include <limits.h>

TEST_GROUP(Math)
    int i;
    long l;
    long long ll;
    float f;
    double d;
    long double ld;
    TEST_TRACE(C99 7.12)
    TEST_TRACE(C99 7.12/2)
    float_t ft = 1.0;
    double_t dt = 1.0;
    TEST_TRACE(C99 7.12/3)
    d = HUGE_VAL;
    f = HUGE_VALF;
    ld = HUGE_VALL;
    TEST_TRACE(C99 7.12/4)
    f = INFINITY;
    TEST_TRACE(C99 7.12/5)
    f = NAN;
    TEST_TRACE(C99 7.12/6)
    TEST(FP_INFINITE != FP_NAN
      && FP_INFINITE != FP_NORMAL
      && FP_INFINITE != FP_SUBNORMAL
      && FP_INFINITE != FP_ZERO, "FP_INFINITE has a distinct value");
    TEST(FP_NAN != FP_NORMAL
      && FP_NAN != FP_SUBNORMAL
      && FP_NAN != FP_ZERO, "FP_NAN has a distinct value");
    TEST(FP_NORMAL != FP_SUBNORMAL
      && FP_NORMAL != FP_ZERO, "FP_NORMAL has a distinct value");
    TEST(FP_SUBNORMAL != FP_ZERO, "FP_SUBNORMAL has a distinct value");
    TEST_TRACE(C99 7.12/7)
    // No FP_FAST_FMA
    TEST_TRACE(C99 7.12/8)
    TEST(FP_ILOGB0 == -INT_MAX || FP_ILOGB0 == INT_MIN, "FP_ILOGB0 is valid");
    TEST(FP_ILOGBNAN == INT_MAX || FP_ILOGBNAN == INT_MIN, "FP_ILOGBNAN is valid");
    TEST_TRACE(C99 7.12/9)
    TEST(MATH_ERRNO == 1, "MATH_ERRNO is valid");
    TEST(MATH_ERREXCEPT == 2, "MATH_ERREXCEPT is valid");
    i = math_errhandling;
    TEST_TRACE(C99 7.12.3.1)
    TEST(fpclassify(0.0) == FP_ZERO, "fpclassify(1.0) == FP_ZERO");
    TEST_EXCLUDE(ARMEB, "http://ellcc.org/bugzilla/show_bug.cgi?id=14")
        TEST(fpclassify(1.0) == FP_NORMAL, "fpclassify(1.0) == FP_NORMAL");
        TEST_FAIL(ARMEB, fpclassify(1.0) == FP_NORMAL, "fpclassify(1.0) == FP_NORMAL");
    TEST(fpclassify(NAN) == FP_NAN, "fpclassify(NAN) == FP_NAN");
    TEST(fpclassify(INFINITY) == FP_INFINITE, "fpclassify(INFINITY) == FP_INFINITE");
    TEST_TRACE(C99 7.12.3.2)
    TEST(isfinite(0.0), "isfinite(0.0) is true");
    TEST(isfinite(1.0), "isfinite(1.0) is true");
    TEST(!isfinite(NAN), "isfinite(NAN) is false");
    TEST(!isfinite(INFINITY), "isfinite(INFINITY) is false");
    TEST_TRACE(C99 7.12.3.3)
    TEST(!isinf(0.0), "isinf(0.0) is false");
    TEST(!isinf(1.0), "isinf(1.0) is false");
    TEST(!isinf(NAN), "isinf(NAN) is false");
    TEST(isinf(INFINITY), "isinf(INFINITY) is true");
    TEST_TRACE(C99 7.12.3.4)
    TEST(!isnan(0.0), "isnan(0.0) is false");
    TEST(!isnan(1.0), "isnan(1.0) is false");
    TEST(isnan(NAN), "isnan(NAN) is true");
    TEST(!isnan(INFINITY), "isnan(INFINITY) is false");
    TEST_TRACE(C99 7.12.3.5)
    TEST(!isnormal(0.0), "isnormal(0.0) is false");
    TEST(isnormal(1.0), "isnormal(1.0) is true");
    TEST(!isnormal(NAN), "isnormal(NAN) is false");
    TEST(!isnormal(INFINITY), "isnormal(INFINITY) is false");
    TEST_TRACE(C99 7.12.3.6)
    TEST(!signbit(1.0), "signbit(1.0) is false");
    TEST(signbit(-1.0), "signbit(-1.0) is true");
    TEST_TRACE(C99 7.12.4.1)
    d = acos(0.0);
    f = acosf(0.0);
    ld = acosl(0.0);
    TEST_TRACE(C99 7.12.4.2)
    d = asin(0.0);
    f = asinf(0.0);
    ld = asinl(0.0);
    TEST_TRACE(C99 7.12.4.3)
    d = atan(0.0);
    f = atanf(0.0);
    ld = atanl(0.0);
    TEST_EXCLUDE(MIPS, "http://ellcc.org/bugzilla/show_bug.cgi?id=59") {
        TEST_TRACE(C99 7.12.4.4)
        d = atan2(1.0, 2.0);
        f = atan2f(1.0, 2.0);
        ld = atan2l(1.0, 2.0);
    }
    TEST_TRACE(C99 7.12.4.5)
    d = cos(0.0);
    f = cosf(0.0);
    ld = cosl(0.0);
    TEST_TRACE(C99 7.12.4.6)
    d = sin(0.0);
    f = sinf(0.0);
    ld = sinl(0.0);
    TEST_TRACE(C99 7.12.4.7)
    d = tan(0.0);
    f = tanf(0.0);
    ld = tanl(0.0);
    TEST_TRACE(C99 7.12.5.1)
    d = acosh(1.0);
    f = acoshf(1.0);
    ld = acoshl(1.0);
    TEST_TRACE(C99 7.12.5.2)
    d = asinh(0.0);
    f = asinhf(0.0);
    ld = asinhl(0.0);
    TEST_TRACE(C99 7.12.5.3)
    d = atanh(0.0);
    f = atanhf(0.0);
    ld = atanhl(0.0);
    TEST_TRACE(C99 7.12.5.4)
    d = cosh(0.0);
    f = coshf(0.0);
    ld = coshl(0.0);
    TEST_TRACE(C99 7.12.5.5)
    d = sinh(0.0);
    f = sinhf(0.0);
    ld = sinhl(0.0);
    TEST_TRACE(C99 7.12.5.6)
    d = tanh(0.0);
    f = tanhf(0.0);
    ld = tanhl(0.0);
    TEST_TRACE(C99 7.12.6.1)
    d = exp(0.0);
    f = expf(0.0);
    ld = expl(0.0);
    TEST_TRACE(C99 7.12.6.2)
    d = exp2(0.0);
    f = exp2f(0.0);
    ld = exp2l(0.0);
    TEST_TRACE(C99 7.12.6.3)
    d = expm1(0.0);
    f = expm1f(0.0);
    ld = expm1l(0.0);
    TEST_TRACE(C99 7.12.6.4)
    d = frexp(0.0, &i);
    f = frexpf(0.0, &i);
    ld = frexpl(0.0, &i);
    TEST_TRACE(C99 7.12.6.5)
    d = ilogb(0.0);
    f = ilogbf(0.0);
    ld = ilogbl(0.0);
    TEST(ilogb(0.0) == FP_ILOGB0, "ilogb(0.0) == FP_ILOGB0");
    TEST_RESOLVED(ARM, "http://ellcc.org/bugzilla/show_bug.cgi?id=14")
    TEST_EXCLUDE(ARMEB, "http://ellcc.org/bugzilla/show_bug.cgi?id=14")
        TEST(ilogb(NAN) == FP_ILOGBNAN, "ilogb(NAN) == FP_ILOGBNAN");
        TEST_FAIL(ARMEB, ilogb(NAN) == FP_ILOGBNAN, "ilogb(NAN) == FP_ILOGBNAN");
    TEST_TRACE(C99 7.12.6.6)
    d = ldexp(1.0, 1);
    f = ldexpf(1.0, 1);
    ld = ldexpl(1.0, 1);
    TEST_RESOLVED(ARM, "http://ellcc.org/bugzilla/show_bug.cgi?id=14")
    TEST_EXCLUDE(ARMEB, "http://ellcc.org/bugzilla/show_bug.cgi?id=14")
        TEST(ldexp(1.0, 1) == 2.0, "ldexp(1.0, 1) == 2.0");
        TEST_FAIL(ARMEB, ldexp(1.0, 1) == 2.0, "ldexp(1.0, 1) == 2.0");
    TEST_TRACE(C99 7.12.6.7)
    d = log(1.0);
    f = logf(1.0);
    ld = logl(1.0);
    TEST_TRACE(C99 7.12.6.8)
    d = log10(1.0);
    f = log10f(1.0);
    ld = log10l(1.0);
    TEST_EXCLUDE(MIPS, "http://ellcc.org/bugzilla/show_bug.cgi?id=59") {
        TEST_TRACE(C99 7.12.6.9)
        d = log1p(1.0);
        f = log1pf(1.0);
        ld = log1pl(1.0);
    }
    TEST_TRACE(C99 7.12.6.10)
    d = log2(1.0);
    f = log2f(1.0);
    ld = log2l(1.0);
    TEST_TRACE(C99 7.12.6.11)
    d = logb(1.0);
    f = logbf(1.0);
    ld = logbl(1.0);
    TEST_TRACE(C99 7.12.6.12)
    d = modf(0.0, &d);
    f = modff(0.0, &f);
    ld = modfl(0.0, &ld);
    TEST_TRACE(C99 7.12.6.13)
    d = scalbn(1.0, 1);
    f = scalbnf(1.0, 1);
    ld = scalbnl(1.0, 1);
    d = scalbln(1.0, 1);
    f = scalblnf(1.0, 1);
    ld = scalblnl(1.0, 1);
    TEST_TRACE(C99 7.12.7.1)
    d = cbrt(0.0);
    f = cbrtf(0.0);
    ld = cbrtl(0.0);
    TEST_TRACE(C99 7.12.7.2)
    d = fabs(0.0);
    f = fabsf(0.0);
    ld = fabsl(0.0);
    TEST_EXCLUDE(MIPS, "http://ellcc.org/bugzilla/show_bug.cgi?id=59") {
        TEST_TRACE(C99 7.12.7.3)
        d = hypot(2.0, 2.0);
        f = hypotf(2.0, 2.0);
        ld = hypotl(2.0, 2.0);
    }
    TEST_TRACE(C99 7.12.7.4)
    d = pow(2.0, 2.0);
    f = powf(2.0, 2.0);
    ld = powl(2.0, 2.0);
    TEST_TRACE(C99 7.12.7.5)
    d = sqrt(4.0);
    f = sqrtf(4.0);
    ld = sqrtl(4.0);
    TEST_EXCLUDE(MIPS, "http://ellcc.org/bugzilla/show_bug.cgi?id=59") {
        TEST_TRACE(C99 7.12.8.1)
        d = erf(0.0);
        f = erff(0.0);
        ld = erfl(0.0);
    }
    TEST_TRACE(C99 7.12.8.2)
    d = erfc(0.0);
    f = erfcf(0.0);
    ld = erfcl(0.0);
    TEST_TRACE(C99 7.12.8.3)
    d = lgamma(1.0);
    f = lgammaf(1.0);
    ld = lgammal(1.0);
    TEST_TRACE(C99 7.12.8.4)
    d = tgamma(1.0);
    f = tgammaf(1.0);
    ld = tgammal(1.0);
    TEST_TRACE(C99 7.12.9.1)
    d = ceil(1.0);
    f = ceilf(1.0);
    ld = ceill(1.0);
    TEST_TRACE(C99 7.12.9.2)
    d = floor(1.0);
    f = floorf(1.0);
    ld = floorl(1.0);
    TEST_EXCLUDE(MIPS, "http://ellcc.org/bugzilla/show_bug.cgi?id=59") {
        TEST_TRACE(C99 7.12.9.3)
        d = nearbyint(1.0);
        f = nearbyintf(1.0);
        ld = nearbyintl(1.0);
        TEST_TRACE(C99 7.12.9.4)
        d = rint(1.0);
        f = rintf(1.0);
        ld = rintl(1.0);
        TEST_TRACE(C99 7.12.9.5)
        l = lrint(1.0);
        l = lrintf(1.0);
        l = lrintl(1.0);
        ll = llrint(1.0);
        ll = llrintf(1.0);
        ll = llrintl(1.0);
    }
    TEST_TRACE(C99 7.12.9.6)
    d = round(1.0);
    f = roundf(1.0);
    ld = roundl(1.0);
    TEST_TRACE(C99 7.12.9.7)
    l = lround(1.0);
    l = lroundf(1.0);
    l = lroundl(1.0);
    ll = llround(1.0);
    ll = llroundf(1.0);
    ll = llroundl(1.0);
    TEST_TRACE(C99 7.12.9.6)
    d = trunc(1.0);
    f = truncf(1.0);
    ld = truncl(1.0);
    TEST_EXCLUDE(MIPS, "http://ellcc.org/bugzilla/show_bug.cgi?id=59") {
        TEST_TRACE(C99 7.12.10.1)
        d = fmod(2.0, 2.0);
        f = fmodf(2.0, 2.0);
        ld = fmodl(2.0, 2.0);
    }
    TEST_TRACE(C99 7.12.10.2)
    d = remainder(2.0, 2.0);
    f = remainderf(2.0, 2.0);
    ld = remainderl(2.0, 2.0);
    TEST_EXCLUDE(MIPS, "http://ellcc.org/bugzilla/show_bug.cgi?id=59") {
        TEST_TRACE(C99 7.12.10.3)
        d = remquo(4.0, 2.0, &i);
        f = remquof(4.0, 2.0, &i);
        ld = remquol(4.0, 2.0, &i);
    }
    TEST_TRACE(C99 7.12.11.1)
    d = copysign(4.0, -2.0);
    f = copysignf(4.0, -2.0);
    ld = copysignl(4.0, -2.0);
    TEST_TRACE(C99 7.12.11.2)
    d = nan("");
    f = nanf("");
    ld = nanl("");
    TEST_TRACE(C99 7.12.11.3)
    d = nextafter(2.0, 0.0);
    f = nextafterf(2.0, 0.0);
    TEST_EXCLUDE(PPC, "http://ellcc.org/bugzilla/show_bug.cgi?id=47")
    TEST_EXCLUDE(PPC64, "http://ellcc.org/bugzilla/show_bug.cgi?id=47")
        ld = nextafterl(2.0, 0.0);
    TEST_TRACE(C99 7.12.11.4)
    d = nexttoward(2.0, 0.0);
    f = nexttowardf(2.0, 0.0);
    ld = nexttowardl(2.0, 0.0);
    TEST_TRACE(C99 7.12.12.1)
    d = fdim(2.0, 0.0);
    f = fdimf(2.0, 0.0);
    ld = fdiml(2.0, 0.0);
    TEST_TRACE(C99 7.12.12.2)
    d = fmax(2.0, 0.0);
    f = fmaxf(2.0, 0.0);
    TEST_EXCLUDE(PPC, "http://ellcc.org/bugzilla/show_bug.cgi?id=47")
    TEST_EXCLUDE(PPC64, "http://ellcc.org/bugzilla/show_bug.cgi?id=47")
        ld = fmaxl(2.0, 0.0);
    TEST_TRACE(C99 7.12.12.3)
    d = fmin(2.0, 0.0);
    f = fminf(2.0, 0.0);
    TEST_EXCLUDE(PPC, "http://ellcc.org/bugzilla/show_bug.cgi?id=47")
    TEST_EXCLUDE(PPC64, "http://ellcc.org/bugzilla/show_bug.cgi?id=47")
        ld = fminl(2.0, 0.0);
    TEST_TRACE(C99 7.12.13.1)
    d = fma(2.0, 1.0, 6.0);
    f = fmaf(2.0, 1.0, 6.0);
    ld = fmal(2.0, 1.0, 6.0);
    TEST_TRACE(C99 7.12.14.1)
    i = isgreater(2.0, 0.0);
    i = isgreater(2.0f, 0.0f);
    i = isgreater(2.0l, 0.0l);
    TEST_TRACE(C99 7.12.14.2)
    i = isgreaterequal(2.0, 0.0);
    i = isgreaterequal(2.0f, 0.0f);
    i = isgreaterequal(2.0l, 0.0l);
    TEST_TRACE(C99 7.12.14.3)
    i = isless(2.0, 0.0);
    i = isless(2.0f, 0.0f);
    i = isless(2.0l, 0.0l);
    TEST_TRACE(C99 7.12.14.4)
    i = islessequal(2.0, 0.0);
    i = islessequal(2.0f, 0.0f);
    i = islessequal(2.0l, 0.0l);
    TEST_TRACE(C99 7.12.14.5)
    i = islessgreater(2.0, 0.0);
    i = islessgreater(2.0f, 0.0f);
    i = islessgreater(2.0l, 0.0l);
    TEST_TRACE(C99 7.12.14.6)
    i = isunordered(2.0, 0.0);
    i = isunordered(2.0f, 0.0f);
    i = isunordered(2.0l, 0.0l);
END_GROUP
