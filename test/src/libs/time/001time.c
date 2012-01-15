#include <ecc_test.h>
#include <time.h>

TEST_GROUP(Time)
    char *p;
    TEST_TRACE(C99 7.23/3)
    size_t size;
    clock_t c;
    time_t t;
    struct tm tm;
    TEST_TRACE(C99 7.23/2)
    p = NULL;
    c = CLOCKS_PER_SEC;
    TEST_TRACE(C99 7.23/4)
    const struct tm ctm = {
        .tm_sec = 59,
        .tm_min = 9,
        .tm_hour = 13,
        .tm_mday = 14,
        .tm_mon = 0,
        .tm_year = 2012 - 1900,
        .tm_wday = 6,
        .tm_yday = 11,
        .tm_isdst = 0,
    };
    TEST_TRACE(C99 7.23.2.1)
    c = clock();
    TEST_TRACE(C99 7.23.2.3)
    tm = ctm;
    TEST_EXCLUDE(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=30")
    TEST_EXCLUDE(PPC64, "http://ellcc.org/bugzilla/show_bug.cgi?id=31")
    TEST_EXCLUDE(PPC, "http://ellcc.org/bugzilla/show_bug.cgi?id=32")
        t = mktime(&tm);
    TEST_TRACE(C99 7.23.2.3)
    TEST(time(&t) == t, "time() directly and indirectly returns the time");
    TEST_TRACE(C99 7.23.2.2)
    TEST(difftime(t, t) == 0.0, "difftime() with identical times is 0.0");
    TEST_TRACE(C99 7.23.3.1)
    TEST_EXCLUDE(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=33")
    TEST_EXCLUDE(PPC64, "http://ellcc.org/bugzilla/show_bug.cgi?id=34")
    TEST_EXCLUDE(PPC, "http://ellcc.org/bugzilla/show_bug.cgi?id=35") {
        p = asctime(&ctm);
        TEST(strcmp(p, "Sat Jan 14 13:09:59 2012\n") == 0, "asctime() succeeds");
    }
END_GROUP
