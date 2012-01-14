#include <ecc_test.h>
#include <tgmath.h>

TEST_GROUP(Tgmath)
    TEST_TRACE(C99 7.22/7)
    int n = 0;
    float f = 0;
    double d = 0;
    long double ld = 0;
    float complex fc = 0;
    double complex dc = 0;
    long double complex ldc = 0;
    f = acos(f);
    d = acos(d);
    // RICH: ld = acos(ld);
    fc = acos(fc);
    dc = acos(dc);
    // RICH: ldc = acos(ldc);
    f = asin(f);
    d = asin(d);
    // RICH: ld = asin(ld);
    fc = asin(fc);
    dc = asin(dc);
    // RICH: ldc = asin(ldc);
    f = atan(f);
    d = atan(d);
    // RICH: ld = atan(ld);
    fc = atan(fc);
    dc = atan(dc);
    // RICH: ldc = atan(ldc);
END_GROUP
