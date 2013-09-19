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
#include <stddef.h>

TEST_GROUP(Stddef)
    TEST_TRACE(C99 7.17/2)
    const char *p = "hi";
    ptrdiff_t d = p - p;
    TEST(d == 0, "A pointer is equal to itself");
    size_t s = sizeof(d);
    TEST_TRACE(C99 7.17/3)
    p = NULL;
    struct st {
        char a, b;
    };
    TEST(offsetof(struct st, a) == 0, "offsetof(a) works");
    TEST(offsetof(struct st, b) == 1, "offsetof(b) works");
END_GROUP

