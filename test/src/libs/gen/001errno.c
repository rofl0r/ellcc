#include <ecc_test.h>
#include <errno.h>

TEST_GROUP(Errno)
    // C99 7.5
    TEST(EDOM > 0, "EDOM is positive");
    TEST(EILSEQ > 0, "EILSEQ is positive");
    TEST(ERANGE > 0, "ERANGE is positive");
    TEST(EDOM != EILSEQ && EDOM != ERANGE && EILSEQ != ERANGE, "Error values are distinct");
    errno = 0;
    TEST(errno == 0, "errno is an lvalue");
    errno = 1;
    TEST(errno == 1, "errno is a modifiable lvalue");
END_GROUP

