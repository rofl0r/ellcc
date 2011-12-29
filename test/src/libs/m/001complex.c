#include <ecc_test.h>
#include <complex.h>

// The type imaginary is not supported.
TEST_GROUP(Complex)
    float complex f = I;
    double complex d = I;
    long double complex ld = I;
    d = cacos(d);
    f = cacosf(f);
    // RICH: ld = cacosl(ld);
    
END_GROUP
