#include <ecc_test.h>
#include <complex.h>

// The type imaginary is not supported.
TEST_GROUP(Complex)
    float complex f = I;
    double complex d;
    long double complex ld;
    d = cacos(d);
END_GROUP
