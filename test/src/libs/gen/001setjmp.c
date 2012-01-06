#include <ecc_test.h>
#include <setjmp.h>

TEST_GROUP(Setjmp)
    int i = 0;
    TEST_TRACE(C99 7.13/2)
    jmp_buf buf;
    TEST_TRACE(C99 7.13.1.1)
    if (setjmp(buf)) {
        // Entered via longjmp().
        i = 1;
    }
    TEST_TRACE(C99 7.13.2.1)
    if (i == 0) {
        longjmp(buf, 1);
    }
    TEST(i == 1, "setjmp/longjmp works");
END_GROUP

