#include "../ecc_test.h"
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
#include <errno.h>

TEST_GROUP(Errno)
    TEST_TRACE(C99 7.5)
    TEST(EDOM > 0, "EDOM is positive");
    TEST(EILSEQ > 0, "EILSEQ is positive");
    TEST(ERANGE > 0, "ERANGE is positive");
    TEST(EDOM != EILSEQ && EDOM != ERANGE && EILSEQ != ERANGE, "Error values are distinct");
    errno = 0;
    TEST(errno == 0, "errno is an lvalue");
    errno = 1;
    TEST(errno == 1, "errno is a modifiable lvalue");
END_GROUP

