#include <ecc_test.h>
#include <stdlib.h>
#include <limits.h>

static void func(void)
{
}

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
    TEST_TRACE(C99 7.20.3.1)
    p = calloc(100, sizeof(char));
    TEST(p != NULL, "calloc() returned a pointer");
    int flag = 1;
    for (i = 0; i < 100; ++i) {
        if (p[i] != 0) {
            flag = 0;
        }
    }
    TEST(flag, "calloc() returned zeroed memory");
    TEST_TRACE(C99 7.20.3.2)
    free(p);
    free(NULL);
    TEST_TRACE(C99 7.20.3.3)
    p = malloc(100);
    TEST(p != NULL, "malloc() returned a pointer");
    TEST_TRACE(C99 7.20.3.4)
    p = realloc(p, 200);
    TEST(p != NULL, "realloc() returned a pointer");
    free(p);
    TEST_TRACE(C99 7.20.1.1)
    void (*ap)(void) = abort;
    TEST_EXCLUDE(ALL_PROCESSORS, "http://ellcc.org/bugzilla/show_bug.cgi?id=25")
        TEST((abort(), 1), "abort()");
    TEST_TRACE(C99 7.20.1.2)
    TEST(atexit(func) == 0, "func is registered with atexit()");
    TEST_TRACE(C99 7.20.1.3)
    void (*ep)(int) = exit;
    TEST_EXCLUDE(ALL_PROCESSORS, "http://ellcc.org/bugzilla/show_bug.cgi?id=26")
        TEST((exit(0), 1), "exit()");
    TEST_TRACE(C99 7.20.1.4)
    ep = _Exit;
    TEST_EXCLUDE(ALL_PROCESSORS, "http://ellcc.org/bugzilla/show_bug.cgi?id=27")
        TEST((_Exit(0), 1), "_Exit()");
END_GROUP
