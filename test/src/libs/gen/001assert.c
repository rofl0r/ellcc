#include <ecc_test.h>

TEST_GROUP(Assert)
    // C99 7.2.1.1
    #include <assert.h>
    TEST((assert(1), 1), "Assertion true");
    // C99 7.2
    #define NDEBUG
    #include <assert.h>
    TEST((assert(0), 1), "Assertion false and NDEBUG");
END_GROUP

