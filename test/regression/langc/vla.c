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

static void g(int a)
{
}

static void f(int size)
{
    TEST_TRACE(C99 6.7.5.2)
    char array[size];
    array[0] = 1;
    g(0);
    TEST_EXCLUDE(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=29")
        TEST(array[0] == 1, "VLA assignment worked %d", array[0]);
    TEST_FAIL(MICROBLAZE, array[0] == 1, "VLA assignment worked %d", array[0]);
}

TEST_GROUP(VLA)
    f(1);
END_GROUP

