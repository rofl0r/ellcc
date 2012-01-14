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
    tm.tm_sec = 59;
    tm.tm_min = 9;
    tm.tm_hour = 13;
    tm.tm_mday = 14;
    tm.tm_mon = 0;
    tm.tm_year = 112;
    tm.tm_wday = 6;
    tm.tm_yday = 11;
    tm.tm_isdst = 1;
    TEST_TRACE(C99 7.23.2.1)
    c = clock();
END_GROUP
