#include <ecc_test.h>

static void g(char *array, int size)
{
    for (int i = 0; i < size; ++i) {
        array[i] = size - i;
    }
}

static void f(int size)
{
    TEST_TRACE(C99 6.7.5.2)
    char array[size];
    g(array, size);
    TEST(array[0] == 1, "VLA assignment worked");
}

TEST_GROUP(VLA)
    TEST_EXCLUDE(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=29")
        f(1);
END_GROUP

