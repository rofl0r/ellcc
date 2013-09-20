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
#include <stdint.h>

#define HAS(type, bytes) defined(__ ## type ## bytes ## _TYPE__)
TEST_GROUP(Stdint)
    TEST_TRACE(C99 7.18.1.1)
    // N.B. We assume 2's complement.
#if HAS(INT, 8)
    int8_t int8 = 0x80;
    uint8_t uint8 = ~0;
    TEST(int8 == -128, "int8_t is 8 bits");
    TEST(uint8 == 0xFF, "uint8_t is 8 bits");
#endif
#if HAS(INT, 16)
    int16_t int16 = 0x8000;
    uint16_t uint16 = ~0;
    TEST(int16 == -32768, "int16_t is 16 bits");
    TEST(uint16 == 0xFFFF, "uint16_t is 16 bits");
#endif
#if HAS(INT, 32)
    int32_t int32 = 0x80000000;
    uint32_t uint32 = ~0;
    TEST(int32 == -2147483648, "int32_t is 32 bits");
    TEST(uint32 == 0xFFFFFFFF, "uint32_t is 32 bits");
#endif
#if HAS(INT, 64)
    int64_t int64 = 0x8000000000000000LL;
    uint64_t uint64 = ~0;
    TEST(int64 == -9223372036854775808ULL, "int64_t is 64 bits");
    TEST(uint64 == 0xFFFFFFFFFFFFFFFFLL, "uint64_t is 64 bits");
    int64 = 1234567890123456789LL;
    TEST(int64 % 10 == 9, "int64_t modulus");
    TEST(int64 / 10 == 123456789012345678LL, "int64_t divide");
    // From NetBSD localtime.c.
    int i;
    int64_t lo, hi;
    lo = 1;
    for (i = 0; i < (int) (__CHAR_BIT__ * sizeof(lo) - 1); ++i)
            lo *= 2;
    hi = -(lo + 1);
    TEST(lo == -9223372036854775808ULL, "lo is correct: %lld", (long long)lo);
    TEST(hi == 9223372036854775807LL, "hi is correct: %lld", (long long)hi);

#endif
    TEST_TRACE(C99 7.18.1.2)
    int_least8_t int_least8 = 0;
    uint_least8_t uint_least8 = 0;
    int_least16_t int_least16 = 0;
    uint_least16_t uint_least16 = 0;
    int_least32_t int_least32 = 0;
    uint_least32_t uint_least32 = 0;
    int_least64_t int_least64 = 0;
    uint_least64_t uint_least64 = 0;
    TEST_TRACE(C99 7.18.1.3)
    int_fast8_t int_fast8 = 0;
    uint_fast8_t uint_fast8 = 0;
    int_fast16_t int_fast16 = 0;
    uint_fast16_t uint_fast16 = 0;
    int_fast32_t int_fast32 = 0;
    uint_fast32_t uint_fast32 = 0;
    int_fast64_t int_fast64 = 0;
    uint_fast64_t uint_fast64 = 0;
    TEST_TRACE(C99 7.18.1.4)
    const void *p = "hi";
    intptr_t intptr = (intptr_t)p;
    TEST((void *)intptr == p, "intptr_t works");
    uintptr_t uintptr = (uintptr_t)p;
    TEST((void *)uintptr == p, "uintptr_t works");
    TEST_TRACE(C99 7.18.1.4)
    intmax_t intmax = 0;
    uintmax_t uintmax = 0;
    TEST_TRACE(C99 7.18.2.1)
#if HAS(INT, 8)
    int8 = INT8_MIN;
    int8 = INT8_MAX;
    uint8 = UINT8_MAX;
#endif
#if HAS(INT, 16)
    int16 = INT16_MIN;
    int16 = INT16_MAX;
    uint16 = UINT16_MAX;
#endif
#if HAS(INT, 32)
    int32 = INT32_MIN;
    int32 = INT32_MAX;
    uint32 = UINT32_MAX;
#endif
#if HAS(INT, 64)
    int64 = INT64_MIN;
    int64 = INT64_MAX;
    uint64 = UINT64_MAX;
#endif
    TEST_TRACE(C99 7.18.2.2)
#if HAS(INT_LEAST, 8)
    int_least8 = INT_LEAST8_MIN;
    int_least8 = INT_LEAST8_MAX;
    uint_least8 = UINT_LEAST8_MAX;
#endif
#if HAS(INT_LEAST, 16)
    int_least16 = INT_LEAST16_MIN;
    int_least16 = INT_LEAST16_MAX;
    uint_least16 = UINT_LEAST16_MAX;
#endif
#if HAS(INT_LEAST, 32)
    int_least32 = INT_LEAST32_MIN;
    int_least32 = INT_LEAST32_MAX;
    uint_least32 = UINT_LEAST32_MAX;
#endif
#if HAS(INT_LEAST, 64)
    int_least64 = INT_LEAST64_MIN;
    int_least64 = INT_LEAST64_MAX;
    uint_least64 = UINT_LEAST64_MAX;
#endif
    TEST_TRACE(C99 7.18.2.3)
#if HAS(INT_FAST, 8)
    int_fast8 = INT_FAST8_MIN;
    int_fast8 = INT_FAST8_MAX;
    uint_fast8 = UINT_FAST8_MAX;
#endif
#if HAS(INT_FAST, 16)
    int_fast16 = INT_FAST16_MIN;
    int_fast16 = INT_FAST16_MAX;
    uint_fast16 = UINT_FAST16_MAX;
#endif
#if HAS(INT_FAST, 32)
    int_fast32 = INT_FAST32_MIN;
    int_fast32 = INT_FAST32_MAX;
    uint_fast32 = UINT_FAST32_MAX;
#endif
#if HAS(INT_FAST, 64)
    int_fast64 = INT_FAST64_MIN;
    int_fast64 = INT_FAST64_MAX;
    uint_fast64 = UINT_FAST64_MAX;
#endif
    TEST_TRACE(C99 7.18.2.4)
    intptr = INTPTR_MIN;
    intptr = INTPTR_MAX;
    uintptr = UINTPTR_MAX;
    TEST_TRACE(C99 7.18.2.5)
    intmax = INTMAX_MIN;
    intmax = INTMAX_MAX;
    uintmax = UINTMAX_MAX;
    TEST_TRACE(C99 7.18.3/2)
    TEST(PTRDIFF_MIN <= -65535, "PTRDIFF_MIN is conforming");
    TEST(PTRDIFF_MAX >= 65535, "PTRDIFF_MAX is conforming");
#if SIG_ATOMIC_MIN == 0
    TEST(SIG_ATOMIC_MIN == 0, "SIG_ATOMIC_MIN is conforming");
    TEST(SIG_ATOMIC_MAX >= 255, "SIG_ATOMIC_MAX is conforming");
#else
    TEST(SIG_ATOMIC_MIN <= -127, "SIG_ATOMIC_MIN is conforming");
    TEST(SIG_ATOMIC_MAX >= 127, "SIG_ATOMIC_MAX is conforming");
#endif
    TEST(SIZE_MAX >= 65535, "SIZE_MAX is conforming");
#if WCHAR_MIN == 0
    TEST(WCHAR_MIN == 0, "WCHAR_MIN is conforming");
    TEST(WCHAR_MAX >= 255, "WCHAR_MAX is conforming");
#else
    TEST(WCHAR_MIN <= -127, "WCHAR_MIN is conforming");
    TEST(WCHAR_MAX >= 127, "WCHAR_MAX is conforming");
#endif
#if WINT_MIN == 0
    TEST(WINT_MIN == 0, "WINT_MIN is conforming");
    TEST(WINT_MAX >= 65535, "WINT_MAX is conforming");
#else
    TEST(WINT_MIN <= -32767, "WINT_MIN is conforming");
    TEST(WINT_MAX >= 32767, "WINT_MAX is conforming");
#endif
    TEST_TRACE(C99 7.18.4.1)
    int_least8 = INT8_C(1);
    uint_least8 = UINT8_C(1);
    int_least16 = INT16_C(1);
    uint_least16 = UINT16_C(1);
    int_least32 = INT32_C(1);
    uint_least32 = UINT32_C(1);
    int_least64 = INT64_C(1);
    uint_least64 = UINT64_C(1);
    TEST_TRACE(C99 7.18.4.1)
    intmax = INTMAX_C(1);
    uintmax = UINTMAX_C(1);
END_GROUP

