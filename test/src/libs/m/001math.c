#include <ecc_test.h>
#include <math.h>
#include <limits.h>

TEST_GROUP(Math)
    int i;
    float f;
    double d;
    long double l;
    // C99 7.12
    // C99 7.12/2
    float_t ft = 1.0;
    double_t dt = 1.0;
    // C99 7.12/3
    d = HUGE_VAL;
    f = HUGE_VALF;
    l = HUGE_VALL;
    // C99 7.12/4
    f = INFINITY;
    // C99 7.12/5
    f = NAN;
    // C99 7.12/6
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
    // C99 7.12/7
    // No FP_FAST_FMA
    // C99 7.12/8
    TEST(FP_ILOGB0 == -INT_MAX || FP_ILOGB0 == INT_MIN, "FP_ILOGB0 is valid");
    TEST(FP_ILOGBNAN == INT_MAX || FP_ILOGBNAN == INT_MIN, "FP_ILOGBNAN is valid");
    // C99 7.12/9
    TEST(MATH_ERRNO == 1, "MATH_ERRNO is valid");
    TEST(MATH_ERREXCEPT == 2, "MATH_ERREXCEPT is valid");
    i = math_errorhandling;
    // C99 7.12.3.1
    TEST(fpclassify(0.0) == FP_ZERO, "fpclassify(1.0) == FP_ZERO");
    TEST(fpclassify(1.0) == FP_NORMAL, "fpclassify(1.0) == FP_NORMAL");
    TEST(fpclassify(NAN) == FP_NAN, "fpclassify(NAN) == FP_NAN");
    TEST(fpclassify(INFINITY) == FP_INFINITE, "fpclassify(INFINITY) == FP_INFINITE");
    // C99 7.12.3.2
    TEST(isfinite(0.0), "isfinite(0.0) is true");
    TEST(isfinite(1.0), "isfinite(1.0) is true");
    TEST(!isfinite(NAN), "isfinite(NAN) is false");
    TEST(!isfinite(INFINITY), "isfinite(INFINITY) is false");
    // C99 7.12.3.3
    TEST(!isinf(0.0), "isinf(0.0) is false");
    TEST(!isinf(1.0), "isinf(1.0) is false");
    TEST(!isinf(NAN), "isinf(NAN) is false");
    TEST(isinf(INFINITY), "isinf(INFINITY) is true");
    // C99 7.12.3.4
    TEST(!isnan(0.0), "isnan(0.0) is false");
    TEST(!isnan(1.0), "isnan(1.0) is false");
    TEST(isnan(NAN), "isnan(NAN) is true");
    TEST(!isnan(INFINITY), "isnan(INFINITY) is false");
    // C99 7.12.3.5
    TEST(!isnormal(0.0), "isnormal(0.0) is false");
    TEST(isnormal(1.0), "isnormal(1.0) is true");
    TEST(!isnormal(NAN), "isnormal(NAN) is false");
    TEST(!isnormal(INFINITY), "isnormal(INFINITY) is false");
    // C99 7.12.3.6
    TEST(!signbit(1.0), "signbit(1.0) is false");
    TEST(signbit(-1.0), "signbit(-1.0) is true");
    // C99 7.12.4.1
    d = acos(0.0);
    f = acosf(0.0);
    // RICH: l = acosl(0.0);
    // C99 7.12.4.2
    d = asin(0.0);
    f = asinf(0.0);
    // RICH: l = asinl(0.0);
    // C99 7.12.4.3
    d = atan(0.0);
    f = atanf(0.0);
    // RICH: l = atanl(0.0);
    // C99 7.12.4.4
    d = atan2(1.0, 2.0);
    f = atan2f(1.0, 2.0);
    // RICH: l = atan2l(1.0, 2.0);
    // C99 7.12.4.5
    d = cos(0.0);
    f = cosf(0.0);
    // RICH: l = cosl(0.0);
    // C99 7.12.4.6
    d = sin(0.0);
    f = sinf(0.0);
    // RICH: l = sinl(0.0);
    // C99 7.12.4.7
    d = tan(0.0);
    f = tanf(0.0);
    // RICH: l = tanl(0.0);
    // C99 7.12.5.1
    d = acosh(1.0);
    f = acoshf(1.0);
    // RICH: l = acoshl(1.0);
    // C99 7.12.5.2
    d = asinh(0.0);
    f = asinhf(0.0);
    // RICH: l = asinhl(0.0);
    // C99 7.12.5.3
    d = atanh(0.0);
    f = atanhf(0.0);
    // RICH: l = atanhl(0.0);
    // C99 7.12.5.4
    d = cosh(0.0);
    f = coshf(0.0);
    // RICH: l = coshl(0.0);
    // C99 7.12.5.5
    d = sinh(0.0);
    f = sinhf(0.0);
    // RICH: l = sinhl(0.0);
    // C99 7.12.5.6
    d = tanh(0.0);
    f = tanhf(0.0);
    // RICH: l = tanhl(0.0);
    // C99 7.12.6.1
    d = exp(0.0);
    f = expf(0.0);
    // RICH: l = expl(0.0);
    // C99 7.12.6.2
    d = exp2(0.0);
    f = exp2f(0.0);
    // RICH: l = exp2l(0.0);
    // C99 7.12.6.3
    d = expm1(0.0);
    f = expm1f(0.0);
    // RICH: l = expm1l(0.0);
    // C99 7.12.6.4
    d = frexp(0.0, &i);
    f = frexpf(0.0, &i);
    // RICH: l = frexpl(0.0, &i);
    // C99 7.12.6.5
    d = ilogb(0.0);
    f = ilogbf(0.0);
    // RICH: l = ilogbl(0.0);
    TEST(ilogb(0.0) == FP_ILOGB0, "ilogb(0.0) == FP_ILOGB0");
    TEST(ilogb(NAN) == FP_ILOGBNAN, "ilogb(NAN) == FP_ILOGBNAN");
    // C99 7.12.6.6
    d = ldexp(1.0, 1);
    f = ldexpf(1.0, 1);
    // RICH: l = ldexpl(1.0, 1);
    TEST(ldexp(1.0, 1) == 2.0, "ldexp(1.0, 1) == 2.0");
    // C99 7.12.6.7
    d = log(0.0);
    f = logf(0.0);
    // RICH: l = logl(0.0);
    // C99 7.12.6.8
    d = log10(0.0);
    f = log10f(0.0);
    // RICH: l = log10l(0.0);
    // C99 7.12.6.9
    d = log1p(0.0);
    f = log1pf(0.0);
    // RICH: l = log1pl(0.0);
    // C99 7.12.6.10
    d = log2(0.0);
    f = log2f(0.0);
    // RICH: l = log2l(0.0);
    // C99 7.12.6.11
    d = logb(0.0);
    f = logbf(0.0);
    // RICH: l = logbl(0.0);
    // C99 7.12.6.12
    d = modf(0.0, &d);
    f = modff(0.0, &f);
    // RICH: l = modfl(0.0, &l);
    // C99 7.12.6.13
    d = scalbn(1.0, 1);
    f = scalbnf(1.0, 1);
    // RICH: l = scalbnl(1.0, 1);
    d = scalbln(1.0, 1);
    f = scalblnf(1.0, 1);
    // RICH: l = scalblnl(1.0, 1);
    // C99 7.12.7.1
    d = cbrt(0.0);
    f = cbrtf(0.0);
    // RICH: l = cbrtl(0.0);
    // C99 7.12.7.2
    d = fabs(0.0);
    f = fabsf(0.0);
    // RICH: l = fabsl(0.0);
    // C99 7.12.7.3
    d = hypot(2.0, 2.0);
    f = hypotf(2.0, 2.0);
    // RICH: l = hypotl(2.0, 2.0);
    // C99 7.12.7.4
    d = pow(2.0, 2.0);
    f = powf(2.0, 2.0);
    // RICH: l = powl(2.0, 2.0);
    // C99 7.12.7.5
    d = sqrt(4.0);
    f = sqrtf(4.0);
    // RICH: l = sqrtl(4.0);
    // C99 7.12.8.1
    d = erf(0.0);
    f = erff(0.0);
    // RICH: l = erfl(0.0);
    // C99 7.12.8.2
    d = erfc(0.0);
    f = erfcf(0.0);
    // RICH: l = erfcl(0.0);
    // C99 7.12.8.3
    d = lgamma(1.0);
    f = lgammaf(1.0);
    // RICH: l = lgammal(1.0);
    // C99 7.12.8.4
    d = tgamma(1.0);
    f = tgammaf(1.0);
    // RICH: l = tgammal(1.0);
END_GROUP
