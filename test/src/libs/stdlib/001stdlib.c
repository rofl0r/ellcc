#include <ecc_test.h>
#include <stdlib.h>
#include <limits.h>

TEST_GROUP(Stdlib)
    float f;
    double d;
    long double ld;
    int i;
    long l;
    long long ll;
    unsigned long long ull;
    char *p;
    TEST_TRACE(C99 7.20)
    TEST_TRACE(C99 7.20/2)
    size_t size;
    wchar_t wchar;
    div_t div;
    ldiv_t ldiv;
    lldiv_t lldiv;
    TEST_TRACE(C99 7.20/2)
    i = EXIT_FAILURE;
    i = EXIT_SUCCESS;
    TEST(RAND_MAX >= 32767, "RAND_MAX >= 32767");
    TEST(MB_CUR_MAX <= MB_LEN_MAX, "MB_CUR_MAX <= MB_LEN_MAX");
    TEST_TRACE(C99 7.20.1.1)
    d = atof("1.0");
    TEST(d == 1.0, "atof(1.0) == 1.0");
    TEST_TRACE(C99 7.20.1.2)
    i = atoi("1234");
    TEST(i == 1234, "atoi(1234) == 1234");
    l = atol("12345678");
    TEST(l == 12345678L, "atol(12345678L) == 12345678L");
    ll = atoll("12345678901234");
    TEST(ll == 12345678901234LL, "atoll(12345678901234LL) == 12345678901234LL");
    TEST_TRACE(C99 7.20.1.3)
    d = strtod("1.0", &p);
    TEST(d == 1.0 && *p == '\0', "strtod(1.0) == 1.0");
    f = strtof("1.0", &p);
    TEST(f == 1.0F && *p == '\0', "strtof(1.0) == 1.0F");
    ld = strtold("1.0", &p);
    TEST(ld == 1.0L && *p == '\0', "strtold(1.0) == 1.0L");
    TEST_TRACE(C99 7.20.1.4)
    l = strtol("12345678", &p, 0);
    TEST(l == 12345678L && *p == '\0', "strtol(12345678L) == 12345678L");
    ll = strtoll("12345678901234", &p, 0);
    TEST(ll == 12345678901234LL && *p == '\0', "strtoll(12345678901234LL) == 12345678901234LL");
    ull = strtoull("12345678901234", &p, 0);
    TEST(ull == 12345678901234LL && *p == '\0', "strtoull(12345678901234LL) == 12345678901234LL");
    TEST_TRACE(C99 7.20.2.1)
    i = rand();
    TEST(i >= 0 && i <= RAND_MAX, "rand() >= 0 && <= RAND_MAX");
    TEST_TRACE(C99 7.20.2.2)
    srand(1);
    TEST(rand() == i, "rand() == srand(1)");
    TEST_EXCLUDE(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=23")
        TEST(rand() != rand(), "rand() != rand()");
END_GROUP
