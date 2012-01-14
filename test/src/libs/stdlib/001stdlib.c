#include <ecc_test.h>
#include <stdlib.h>
#include <limits.h>

static void func(void)
{
}

static int compar(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
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
    div_t rdiv;
    ldiv_t rldiv;
    lldiv_t rlldiv;
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
    TEST_TRACE(C99 7.20.4.1)
    void (*ap)(void) = abort;
    TEST_EXCLUDE(ALL_PROCESSORS, "http://ellcc.org/bugzilla/show_bug.cgi?id=25")
        TEST((abort(), 1), "abort()");
    TEST_TRACE(C99 7.20.4.2)
    TEST(atexit(func) == 0, "func is registered with atexit()");
    TEST_TRACE(C99 7.20.4.3)
    void (*ep)(int) = exit;
    TEST_EXCLUDE(ALL_PROCESSORS, "http://ellcc.org/bugzilla/show_bug.cgi?id=26")
        TEST((exit(0), 1), "exit()");
    TEST_TRACE(C99 7.20.4.4)
    ep = _Exit;
    TEST_EXCLUDE(ALL_PROCESSORS, "http://ellcc.org/bugzilla/show_bug.cgi?id=27")
        TEST((_Exit(0), 1), "_Exit()");
    TEST_TRACE(C99 7.20.4.5)
    TEST(getenv("VERY UNLIKELY NAME") == NULL, "getenv() is here");
    TEST_TRACE(C99 7.20.4.6)
    TEST(system(NULL) != 0, "system() says there is a command interpreter");
    TEST_TRACE(C99 7.20.5)
    int array[] = { 8, 3, 9, 1, 4, 7, 0, 2, 6 };        // 5 is missing.
    #define NMEMB (sizeof(array) / sizeof(array[0]))
    static const int sarray[NMEMB] =  { 0, 1, 2, 3, 4, 6, 7, 8, 9 };
    TEST_TRACE(C99 7.20.5.2)
    qsort(array, NMEMB, sizeof(int), compar);
    TEST(memcmp(array, sarray, sizeof(array)) == 0, "qsort() works");
    TEST_TRACE(C99 7.20.5.1)
    i = 2;
    void *v = bsearch(&i, array, NMEMB, sizeof(int), compar);
    TEST(v != NULL && *(int *)v == 2, "bsearch() finds an existing value");
    i = 5;
    v = bsearch(&i, array, NMEMB, sizeof(int), compar);
    TEST(v == NULL, "bsearch() does not find a non-existant value");
    TEST_TRACE(C99 7.20.6.1)
    TEST(abs(10) == 10, "abs(10) == 10");
    TEST(abs(-10) == 10, "abs(-10) == 10");
    TEST(labs(10L) == 10L, "labs(10L) == 10L");
    TEST(labs(-10L) == 10L, "labs(-10L) == 10L");
    TEST(llabs(10LL) == 10LL, "llabs(10LL) == 10LL");
    TEST(llabs(-10LL) == 10LL, "llabs(-10LL) == 10LL");
    TEST_TRACE(C99 7.20.6.2)
    rdiv = div(13, 5);
    TEST(rdiv.quot == 2 && rdiv.rem == 3, "div(13,5) is 2 rem 3");
    rldiv = ldiv(13L, 5L);
    TEST(rldiv.quot == 2L && rldiv.rem == 3L, "ldiv(13L,5L) is 2L rem 3L");
    TEST_EXCLUDE(I386, "http://ellcc.org/bugzilla/show_bug.cgi?id=28") {
        rlldiv = lldiv(13LL, 5LL);
        TEST(rlldiv.quot == 2LL && rlldiv.rem == 3LL, "lldiv(13LL,5LL) is 2LL rem 3LL");
    }
    TEST_TRACE(C99 7.20.7.1)
    TEST(mblen("", 1) == 0, "mblen(\"\") == 0");
    TEST(mblen("a", 1) == 1, "mblen(\"a\") == 1");
    TEST_TRACE(C99 7.20.7.2)
    TEST(mbtowc(&wchar, "", 1) == 0, "mbtowc(\"\") == 0");
    TEST(wchar == L'\0', "the result is L'\\0'");
    TEST(mbtowc(&wchar, "a", 1) == 1, "mbtowc(\"a\") == 1");
    TEST(wchar == L'a', "the result is L'a'");
    TEST_TRACE(C99 7.20.7.3)
    char mbbuf[MB_CUR_MAX];
    TEST(wctomb(mbbuf, L'a') == 1, "L'a' is a one byte multibyte character");
    TEST_EXCLUDE(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=29")
        TEST(mbbuf[0] == 'a', "L'a' is 'a'");
    TEST_TRACE(C99 7.20.8.1)
    wchar_t wcbuf[10];
    TEST(mbstowcs(wcbuf, "a", 10) == 1, "\"a\" is one multibyte character");
    TEST(wcbuf[0] == L'a', "'a' is L'a'");
    TEST_TRACE(C99 7.20.8.2)
    char buf[100];
    TEST(wcstombs(buf, wcbuf, 100) == 1, "L\"a\" converts to a 1 byte multibyte character");
    TEST(strcmp(buf, "a") == 0, "L\"a\" is equal to \"a\"");
END_GROUP
