#include <ecc_test.h>
#include <stddef.h>

TEST_GROUP(Stddef)
    TEST_TRACE(C99 7.17/2)
    const char *p = "hi";
    ptrdiff_t d = p - p;
    TEST(d == 0, "A pointer is equal to itself");
    size_t s = sizeof(d);
    TEST_TRACE(C99 7.17/3)
    p = NULL;
    struct st {
        char a, b;
    };
    TEST(offsetof(struct st, a) == 0, "offsetof(a) works");
    TEST(offsetof(struct st, b) == 1, "offsetof(b) works");
END_GROUP

