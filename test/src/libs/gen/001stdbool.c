#include <ecc_test.h>
#include <stdbool.h>

TEST_GROUP(Stdbool)
    TEST_TRACE(C99 7.16/2)
    bool flag;
    TEST_TRACE(C99 7.16/3)
    flag = true;
    TEST(flag == 1, "true is equal to 1");
    flag = false;
    TEST(flag == 0, "false is equal to 0");
    TEST(__bool_true_false_are_defined == 1, "__bool_true_false_are_defined is 1");
END_GROUP

