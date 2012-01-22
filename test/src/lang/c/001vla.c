#include <ecc_test.h>

static void g(int a)
{
}

static void f(int size)
{
    TEST_TRACE(C99 6.7.5.2)
    char array[size];
    array[0] = 1;
    g(0);
    TEST(array[0] == 1, "VLA assignment worked %d", array[0]);
}

TEST_GROUP(VLA)
    TEST_RESOLVED(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=29")
        f(1);
END_GROUP

