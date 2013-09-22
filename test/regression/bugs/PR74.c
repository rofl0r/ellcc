// XFAIL: *
// RUN: %mipsecc -o %t %s && %mipsrun %t
#include "../ecc_test.h"
#include <stdarg.h>

static void vfunc3(int count, ...)
{
    va_list ap;
    va_start(ap, count);
    int i, j;
    double d;
    // Consume all int arguments.
    j = 1;
    while (--count) {
        i = va_arg(ap, int);
        ++j;
    }
    d = va_arg(ap, double);
    TEST(d == 3.1415927, "double: %g", d);
    va_end(ap);
}

TEST_GROUP(Stdarg)
    vfunc3(2, 1, 3.1415927);
END_GROUP


