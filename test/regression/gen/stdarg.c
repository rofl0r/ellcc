// Compile and run for every target.
// RUN: %ecc -o %t %s && %t
// RUN: %armecc -o %t %s && %armrun %t
// RUN: %armebecc -o %t %s && %armebrun %t
// RUN: %i386ecc -o %t %s && %i386run %t
// RUN: %microblazeecc -o %t %s && %microblazerun %t
// RUN: %mipsecc -o %t %s && %mipsrun %t
// RUN: %mipselecc -o %t %s && %mipselrun %t
// RUN: %ppcecc -o %t %s && %ppcrun %t
// FAIL: %ppc64ecc -o %t %s && %ppc64run %t
// RUN: %x86_64ecc -o %t %s && %x86_64run %t
#include "../ecc_test.h"
#include <stdarg.h>

static int vfunc(int count, ...)
{
    TEST_TRACE(C99 7.15/3)
    va_list ap;
    int i0, i1, ci1;
    TEST_TRACE(C99 7.15.1.4)
    va_start(ap, count);
    TEST(count == 2, "Have two arguments");
    TEST_TRACE(C99 7.15.1.1)
    i0 = va_arg(ap, int);
    TEST_TRACE(C99 7.15.1.2)
    va_list cap;
    va_copy(cap, ap);
    i1 = va_arg(ap, int);
    TEST_EXCLUDE(PPC, "http://ellcc.org/bugzilla/show_bug.cgi?id=21") {
        ci1 = va_arg(cap, int);
        TEST(i1 == ci1, "A copy of the va_list was made");
    }
    TEST_TRACE(C99 7.15.1.3)
    va_end(ap);
    va_end(cap);
    return i0 + i1;
}

/* This test checks for alignment of 64 bit aruments when preceeded
 * by a variable number of 32 bit arguments.
 * This test assumes int is 32 bits and long long is 64 bits. :-(
 */
static void vfunc2(int count, ...)
{
    va_list ap;
    va_start(ap, count);
    int i, j;
    long long ll;
    TEST(sizeof(int) == 4, "int is 4 bytes");
    TEST(sizeof(long long) == 8, "long long is 8 bytes");
    // Consume all int arguments.
    j = 1;
    while (--count) {
        i = va_arg(ap, int);
        TEST(i == j, "int argument is correct: %d == %d", i, j);
        ++j;
    }
    ll = va_arg(ap, long long);
    TEST_RESOLVED(ARM, "http://ellcc.org/bugzilla/show_bug.cgi?id=52")
        TEST(ll == 1234567890123456789LL, "long long vaarg: %lld", ll);
    va_end(ap);
}

static void vfunc3(int count, ...)
{
    va_list ap;
    va_start(ap, count);
    int i, j;
    double d;
    TEST(sizeof(int) == 4, "int is 4 bytes");
    TEST(sizeof(long long) == 8, "long long is 8 bytes");
    // Consume all int arguments.
    j = 1;
    while (--count) {
        i = va_arg(ap, int);
        TEST(i == j, "int argument is correct: %d == %d", i, j);
        ++j;
    }
    d = va_arg(ap, double);
    TEST_EXCLUDE(MIPSEL, "http://ellcc.org/bugzilla/show_bug.cgi?id=53")
    TEST_EXCLUDE(MIPS, "http://ellcc.org/bugzilla/show_bug.cgi?id=53")
    TEST_RESOLVED(ARM, "http://ellcc.org/bugzilla/show_bug.cgi?id=52")
        TEST(d == 3.1415927, "double: %g", d);
    va_end(ap);
}

TEST_GROUP(Stdarg)
    TEST(vfunc(2, 6800, 9) == 6809, "The 6809 is back");
    vfunc2(2, 1, 1234567890123456789LL);
    vfunc2(3, 1, 2, 1234567890123456789LL);
    vfunc2(4, 1, 2, 3, 1234567890123456789LL);
    vfunc2(5, 1, 2, 3, 4, 1234567890123456789LL);
    vfunc3(2, 1, 3.1415927);
    vfunc3(3, 1, 2, 3.1415927);
    vfunc3(4, 1, 2, 3, 3.1415927);
    vfunc3(5, 1, 2, 3, 4, 3.1415927);
END_GROUP

