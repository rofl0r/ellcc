#include <ecc_test.h>
#include <string.h>

TEST_GROUP(String)
    TEST_TRACE(C99 7.21)
    size_t size;
    char *p = NULL;
    static const char zeros[100];
    static const char src[100] = "abcdef";
    char dst[100];
    TEST_TRACE(C99 7.21.2.1)
    TEST(memcpy(dst, src, 100) == dst, "memcpy() returns the destination");
    TEST_TRACE(C99 7.21.4.1)
    TEST(memcmp(dst, src, 100) == 0, "memcmp() matches");
    dst[0] = 'A';
    TEST(memcmp(dst, src, 100) != 0, "memcmp() doesn't match");
    TEST_TRACE(C99 7.21.6.1)
    TEST(memset(dst, 0, 100) == dst, "memset() returns the destination");
    TEST(memcmp(dst, zeros, 100) == 0, "memcmp() finds all zeros");
    TEST_TRACE(C99 7.21.2.2)
    TEST(memmove(dst, src, 100) == dst, "memmove() returns the destination");
    TEST(memcmp(dst, src, 100) == 0, "memcmp() matches");
    TEST_TRACE(C99 7.21.2.3)
    TEST(memset(dst, 0, 100) == dst, "memset() returns the destination");
    TEST(strcpy(dst, src) == dst, "strcpy() returns the destination");
    TEST_TRACE(C99 7.21.4.2)
    TEST(strcmp(dst, src) == 0, "strcmp() matches");
    dst[0] = 'A';
    TEST(strcmp(dst, src) != 0, "strcmp() doesn't match");
    TEST_TRACE(C99 7.21.2.4)
    // Fill dst with non-zero values.
    TEST(memset(dst, 1, 100) == dst, "memset() returns the destination");
    TEST(strncpy(dst, src, 100) == dst, "strncpy() returns the destination");
    TEST(memcmp(dst, src, 100) == 0, "memcmp() matches");
    TEST_TRACE(C99 7.21.3.1)
    TEST(strcat(dst, src) == dst, "strcat() returns the destination");
    TEST(strcmp(dst, "abcdefabcdef") == 0, "strcmp() matches");
    TEST_TRACE(C99 7.21.3.2)
    TEST(strcpy(dst, src) == dst, "strcpy() returns the destination");
    TEST(strncat(dst, src, 4) == dst, "strncat() returns the destination");
    TEST(strcmp(dst, "abcdefabcd") == 0, "strcmp() matches");
    TEST_TRACE(C99 7.21.4.3)
    TEST(strcpy(dst, src) == dst, "strcpy() returns the destination");
    TEST(strcoll(dst, src) == 0, "strcoll() matches");
    TEST_TRACE(C99 7.21.4.4)
    TEST(strncmp(dst, "abcdzzz", 4) == 0, "strncmp() matches");
    TEST_TRACE(C99 7.21.4.5)
    TEST(strxfrm(dst, src, 100) >= 0, "strxfrm() returns a positive value");
    TEST(strcmp(dst, src) == 0, "strcmp() matches");
END_GROUP
