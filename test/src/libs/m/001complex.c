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
    TEST_UNSUPPORTED(ld = cacosl(ld);)
    TEST_TRACE(C99 7.3.5.2)
    d = casin(d);
    f = casinf(f);
    TEST_UNSUPPORTED(ld = casinl(ld);)
    TEST_TRACE(C99 7.3.5.3)
    d = catan(d);
    f = catanf(f);
    TEST_UNSUPPORTED(ld = catanl(ld);)
    TEST_TRACE(C99 7.3.5.4)
    d = ccos(d);
    f = ccosf(f);
    TEST_UNSUPPORTED(ld = ccosl(ld);)
    TEST_TRACE(C99 7.3.5.5)
    d = csin(d);
    f = csinf(f);
    TEST_UNSUPPORTED(ld = csinl(ld);)
    TEST_TRACE(C99 7.3.5.6)
    d = ctan(d);
    f = ctanf(f);
    TEST_UNSUPPORTED(ld = ctanl(ld);)
    TEST_TRACE(C99 7.3.6.1)
    d = cacosh(d);
    f = cacoshf(f);
    TEST_UNSUPPORTED(ld = cacoshl(ld);)
    TEST_TRACE(C99 7.3.6.2)
    d = casinh(d);
    f = casinhf(f);
    TEST_UNSUPPORTED(ld = casinhl(ld);)
    TEST_TRACE(C99 7.3.6.3)
    d = catanh(d);
    f = catanhf(f);
    TEST_UNSUPPORTED(ld = catanhl(ld);)
    TEST_TRACE(C99 7.3.6.4)
    d = ccosh(d);
    f = ccoshf(f);
    TEST_UNSUPPORTED(ld = ccoshl(ld);)
    TEST_TRACE(C99 7.3.6.5)
    d = csinh(d);
    f = csinhf(f);
    TEST_UNSUPPORTED(ld = csinhl(ld);)
    TEST_TRACE(C99 7.3.6.6)
    d = ctanh(d);
    f = ctanhf(f);
    TEST_UNSUPPORTED(ld = ctanhl(ld);)
    TEST_TRACE(C99 7.3.7.1)
    d = cexp(d);
    f = cexpf(f);
    TEST_UNSUPPORTED(ld = cexpl(ld);)
    TEST_TRACE(C99 7.3.7.2)
    d = clog(d);
    f = clogf(f);
    TEST_UNSUPPORTED(ld = clogl(ld);)
    TEST_TRACE(C99 7.3.8.1)
    d = cabs(d);
    f = cabsf(f);
    TEST_UNSUPPORTED(ld = cabsl(ld);)
    TEST_TRACE(C99 7.3.8.2)
    d = cpow(d, d);
    f = cpowf(f, f);
    TEST_UNSUPPORTED(ld = cpowl(ld, ld);)
    TEST_TRACE(C99 7.3.8.3)
    d = csqrt(d);
    f = csqrtf(f);
    TEST_UNSUPPORTED(ld = csqrtl(ld);)
    TEST_TRACE(C99 7.3.9.1)
    d = carg(d);
    f = cargf(f);
    TEST_UNSUPPORTED(ld = cargl(ld);)
    TEST_TRACE(C99 7.3.9.2)
    d = cimag(d);
    f = cimagf(f);
    TEST_UNSUPPORTED(ld = cimagl(ld);)
    TEST_TRACE(C99 7.3.9.3)
    d = conj(d);
    f = conjf(f);
    TEST_UNSUPPORTED(ld = conjl(ld);)
    TEST_TRACE(C99 7.3.9.4)
    d = cproj(d);
    f = cprojf(f);
    TEST_UNSUPPORTED(ld = cprojl(ld);)
    TEST_TRACE(C99 7.3.9.5)
    double rd = creal(d);
    float rf = crealf(f);
    TEST_UNSUPPORTED(long double rld = creall(ld);)
    
END_GROUP
