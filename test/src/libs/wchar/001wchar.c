#include <ecc_test.h>
#include <wchar.h>

TEST_GROUP(Wchar)
    FILE *f;
    TEST_TRACE(C99 7.24/2)
    wchar_t wchar;
    size_t size;
    mbstate_t mbstate;
    wint_t wint;
    struct tm *tm;
    TEST_TRACE(C99 7.24/3)
    f = NULL;
    wint = WCHAR_MIN;
    wint = WCHAR_MAX;
    wint = WEOF;
    TEST_TRACE(C99 7.242.1)
END_GROUP
