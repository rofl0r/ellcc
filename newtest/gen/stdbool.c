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
#include <stdbool.h>

TEST_GROUP(Stdbool)
    TEST_TRACE(C99 7.16/2)
    bool flag;
    TEST_TRACE(C99 7.16/3)
    flag = true;
    TEST(flag == 1, "true is equal to 1");
    flag = false;
    TEST(flag == 0, "false is equal to 0");
    TEST(__bool_true_false_are_defined == 1, "__bool_true_false_are_defined is 1");
END_GROUP

