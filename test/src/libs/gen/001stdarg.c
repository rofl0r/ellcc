#include <ecc_test.h>
#include <stdarg.h>

int vfunc(int count, ...)
{
    TEST_TRACE(C99 7.15/3)
    va_list ap;
    int i0, i1, ci1;
    TEST_TRACE(C99 7.15.1.4)
    va_start(ap, count);
    TEST(count == 2, "Have two arguments");
    TEST_TRACE(C99 7.15.1.1)
    i0 = va_arg(ap, int);
    TEST_TRACE(C99 7.15.1.2)
    va_list cap;
    va_copy(cap, ap);
    i1 = va_arg(ap, int);
    TEST_EXCLUDE(PPC, "http://ellcc.org/bugzilla/show_bug.cgi?id=21") {
        ci1 = va_arg(cap, int);
        TEST(i1 == ci1, "A copy of the va_list was made");
    }
    TEST_TRACE(C99 7.15.1.3)
    va_end(ap);
    va_end(cap);
    return i0 + i1;
}

TEST_GROUP(Stdarg)
    TEST(vfunc(2, 6800, 9) == 6809, "The 6809 is back");
END_GROUP

