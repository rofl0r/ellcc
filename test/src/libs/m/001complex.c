#include <ecc_test.h>
#include <complex.h>

// The type imaginary is not supported.
TEST_GROUP(Complex)
    float complex f = I;
    double complex d = I;
    long double complex ld = I;
    TEST_TRACE(C99 7.3.5.1)
    d = cacos(d);
    f = cacosf(f);
    // RICH: ld = cacosl(ld);
    TEST_TRACE(C99 7.3.5.2)
    d = casin(d);
    f = casinf(f);
    // RICH: ld = casinl(ld);
    TEST_TRACE(C99 7.3.5.3)
    d = catan(d);
    f = catanf(f);
    // RICH: ld = catanl(ld);
    TEST_TRACE(C99 7.3.5.4)
    d = ccos(d);
    f = ccosf(f);
    // RICH: ld = ccosl(ld);
    TEST_TRACE(C99 7.3.5.5)
    d = csin(d);
    f = csinf(f);
    // RICH: ld = csinl(ld);
    TEST_TRACE(C99 7.3.5.6)
    d = ctan(d);
    f = ctanf(f);
    // RICH: ld = ctanl(ld);
    TEST_TRACE(C99 7.3.6.1)
    d = cacosh(d);
    f = cacoshf(f);
    // RICH: ld = cacoshl(ld);
    TEST_TRACE(C99 7.3.6.2)
    d = casinh(d);
    f = casinhf(f);
    // RICH: ld = casinhl(ld);
    TEST_TRACE(C99 7.3.6.3)
    d = catanh(d);
    f = catanhf(f);
    // RICH: ld = catanhl(ld);
    TEST_TRACE(C99 7.3.6.4)
    d = ccosh(d);
    f = ccoshf(f);
    // RICH: ld = ccoshl(ld);
    TEST_TRACE(C99 7.3.6.5)
    d = csinh(d);
    f = csinhf(f);
    // RICH: ld = csinhl(ld);
    TEST_TRACE(C99 7.3.6.6)
    d = ctanh(d);
    f = ctanhf(f);
    // RICH: ld = ctanhl(ld);
    TEST_TRACE(C99 7.3.7.1)
    d = cexp(d);
    f = cexpf(f);
    // RICH: ld = cexpl(ld);
    TEST_TRACE(C99 7.3.7.2)
    d = clog(d);
    f = clogf(f);
    // RICH: ld = clogl(ld);
    TEST_TRACE(C99 7.3.8.1)
    d = cabs(d);
    f = cabsf(f);
    // RICH: ld = cabsl(ld);
    TEST_TRACE(C99 7.3.8.2)
    d = cpow(d, d);
    f = cpowf(f, f);
    // RICH: ld = cpowl(ld, ld);
    TEST_TRACE(C99 7.3.8.3)
    d = csqrt(d);
    f = csqrtf(f);
    // RICH: ld = csqrtl(ld);
    TEST_TRACE(C99 7.3.9.1)
    d = carg(d);
    f = cargf(f);
    // RICH: ld = cargl(ld);
    TEST_TRACE(C99 7.3.9.2)
    d = cimag(d);
    f = cimagf(f);
    // RICH: ld = cimagl(ld);
    TEST_TRACE(C99 7.3.9.3)
    d = conj(d);
    f = conjf(f);
    // RICH: ld = conjl(ld);
    TEST_TRACE(C99 7.3.9.4)
    d = cproj(d);
    f = cprojf(f);
    // RICH: ld = cprojl(ld);
    TEST_TRACE(C99 7.3.9.5)
    double rd = creal(d);
    float rf = crealf(f);
    // RICH: long double rld = creall(ld);
    
END_GROUP
