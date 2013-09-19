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
#include <limits.h>

#define DOTEST(name, op, value) \
    TEST(name op value, #name " " #op " " #value)
TEST_GROUP(Limits)
    int i;
    TEST_TRACE(C99 7.10)
    TEST_TRACE(C99 5.2.4.2.1)
    DOTEST(CHAR_BIT, >=, 8);
    DOTEST(SCHAR_MIN, <=, -127);
    DOTEST(SCHAR_MAX, >=, 127);
    DOTEST(UCHAR_MAX, >=, 255);
    DOTEST(MB_LEN_MAX, >=, 1);
    DOTEST(SHRT_MIN, <=, -32767);
    DOTEST(SHRT_MAX, >=, 32767);
    DOTEST(USHRT_MAX, >=, 65535);
    DOTEST(INT_MIN, <=, -32767);
    DOTEST(INT_MAX, >=, 32767);
    DOTEST(UINT_MAX, >=, 65535);
    DOTEST(LONG_MIN, <=, -2147483647L);
    DOTEST(LONG_MAX, >=, 2147483647L);
    DOTEST(ULONG_MAX, >=, 4294967295L);
    DOTEST(LLONG_MIN, <=, -9223372036854775807LL);
    DOTEST(LLONG_MAX, >=, -9223372036854775807LL);
    DOTEST(ULLONG_MAX, >=, 18446744073709551615ULL);
    TEST_TRACE(C99 5.2.4.2.1/2)
#ifdef __CHAR_UNSIGNED__
    DOTEST(CHAR_MAX, >=, UCHAR_MAX);
#else
    DOTEST(CHAR_MAX, >=, SCHAR_MAX);
    DOTEST(CHAR_MIN, <=, SCHAR_MIN);
#endif

END_GROUP

