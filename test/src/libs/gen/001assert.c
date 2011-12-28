#include <ecc_test.h>

TEST_GROUP(Assert)
    #include <assert.h>
    TEST((assert(1), 1), "Assertion true");
    #define NDEBUG
    #include <assert.h>
    TEST((assert(0), 1), "Assertion false and NDEBUG");
END_GROUP

