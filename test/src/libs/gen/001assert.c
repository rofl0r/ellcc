#include <ecc_test.h>


#include <assert.h>
static void case1(void)
{
    TEST_TRACE(C99 7.2.1.1)
    TEST((assert(1), 1), "Assertion true");
}

#define NDEBUG
#include <assert.h>
static void case2(void)
{
    TEST_TRACE(C99 7.2)
    TEST((assert(0), 1), "Assertion false and NDEBUG");
}

TEST_GROUP(Assert)
    case1();
    case2();
END_GROUP

