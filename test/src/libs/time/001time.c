#include <ecc_test.h>
#include <time.h>

TEST_GROUP(Time)
    char *p;
    TEST_TRACE(C99 7.23/3)
    size_t size;
    clock_t c;
    time_t t;
    struct tm tm;
    struct tm *rtm;
    char buffer[100];
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
        .tm_yday = 13,
        .tm_isdst = 0,
    };
    TEST_TRACE(C99 7.23.2.1)
    c = clock();
    TEST_TRACE(C99 7.23.2.3)
    tm = ctm;
    TEST_RESOLVED(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=30")
    TEST_EXCLUDE(PPC64, "http://ellcc.org/bugzilla/show_bug.cgi?id=31")
    TEST_EXCLUDE(PPC, "http://ellcc.org/bugzilla/show_bug.cgi?id=32")
    {
        t = mktime(&tm);
        TEST_TRACE(C99 7.23.3.4)
        rtm = localtime(&t);    // NOTE: mktime() assumes tm is in local time.
        TEST(rtm != NULL, "localtime() returns a broken-down time");
        TEST(rtm->tm_sec == ctm.tm_sec, "seconds match");
        TEST(rtm->tm_min == ctm.tm_min, "minutes match");
        TEST(rtm->tm_hour == ctm.tm_hour, "hours match");
        TEST(rtm->tm_mday == ctm.tm_mday, "day of month matches");
        TEST(rtm->tm_mon == ctm.tm_mon, "months match");
        TEST(rtm->tm_year == ctm.tm_year, "years match");
        TEST(rtm->tm_wday == ctm.tm_wday, "week days match");
        TEST(rtm->tm_yday == ctm.tm_yday, "day of year matches");
        TEST(rtm->tm_isdst == ctm.tm_isdst, "daylight savings time matches");
        TEST_TRACE(C99 7.23.3.2)
        p = ctime(&t);
        TEST(strncmp(p, "Sat Jan 14 13:09:59 2012\n", 11) == 0, "ctime(0) succeeds");
        TEST(strcmp(&p[13], ":09:59 2012\n") == 0, "ctime(0) succeeds");
    }
    TEST_TRACE(C99 7.23.2.3)
    TEST(time(&t) == t, "time() directly and indirectly returns the time");
    TEST_TRACE(C99 7.23.2.2)
    TEST(difftime(t, t) == 0.0, "difftime() with identical times is 0.0");
    TEST_TRACE(C99 7.23.3.1)
    TEST_RESOLVED(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=33")
    TEST_EXCLUDE(PPC64, "http://ellcc.org/bugzilla/show_bug.cgi?id=34")
    TEST_EXCLUDE(PPC, "http://ellcc.org/bugzilla/show_bug.cgi?id=35")
    {
        p = asctime(&ctm);
        TEST(strcmp(p, "Sat Jan 14 13:09:59 2012\n") == 0, "asctime() succeeds");
        TEST_TRACE(C99 7.23.3.3)
        t = 0;
        rtm = gmtime(&t);
        p = asctime(rtm);
        TEST(strcmp(p, "Thu Jan  1 00:00:00 1970\n") == 0, "asctime(gmtime(0)) succeeds");
        size = strftime(buffer, sizeof(buffer), "format", &ctm);
        TEST(size == 6, "strftime() returns the number of bytes written");
        TEST(strcmp(buffer, "format") == 0, "strftime() copies non-format characters");
    }
END_GROUP
