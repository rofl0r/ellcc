#include <ecc_test.h>
#include <errno.h>

TEST_GROUP(Errno)
    // 7.5
    TEST(EDOM > 0, "EDOM is positive");
    TEST(EILSEQ > 0, "EILSEQ is positive");
    TEST(ERANGE > 0, "ERANGE is positive");
    TEST(EDOM != EILSEQ && EDOM != ERANGE && EILSEQ != ERANGE, "Error values are unique");
END_GROUP

