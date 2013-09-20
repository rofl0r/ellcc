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
#include <setjmp.h>

TEST_GROUP(Setjmp)
    int i = 0;
    TEST_TRACE(C99 7.13/2)
    jmp_buf buf;
    TEST_TRACE(C99 7.13.1.1)
    if (setjmp(buf)) {
        // Entered via longjmp().
        i = 1;
    }
    TEST_TRACE(C99 7.13.2.1)
    if (i == 0) {
        longjmp(buf, 1);
    }
    TEST(i == 1, "setjmp/longjmp works");
END_GROUP

