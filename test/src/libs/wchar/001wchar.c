#include <ecc_test.h>
#include <wchar.h>

TEST_GROUP(Wchar)
    TEST_TRACE(C99 7.24/2)
    wchar_t wchar;
    size_t size;
    mbstate_t mbstate;
    wint_t wint;
    struct tm *tm;
END_GROUP
