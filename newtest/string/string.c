// Compile and run for every target.
// RUN: %ecc -o %t %s && %t
// RUN: %armecc -o %t %s && %armrun %t
// RUN: %armebecc -o %t %s && %armebrun %t
// RUN: %i386ecc -o %t %s && %i386run %t
// RUN: %microblazeecc -o %t %s && %microblazerun %t
// RUN: %mipsecc -o %t %s && %mipsrun %t
// RUN: %mipselecc -o %t %s && %mipselrun %t
// RUN: %ppcecc -o %t %s && %ppcrun %t
// FAIL: %ppc64ecc -o %t %s && %ppc64run %t
// RUN: %x86_64ecc -o %t %s && %x86_64run %t
#include "../ecc_test.h"
#include <string.h>

TEST_GROUP(String)
    TEST_TRACE(C99 7.21)
    int i;
    size_t size;
    char *p = NULL;
#define CHARBUFSIZ 100
    static const char zeros[CHARBUFSIZ];
    static const char src[CHARBUFSIZ] = "abcdef";
    char dst[CHARBUFSIZ];
    TEST_TRACE(C99 7.21.2.1)
    TEST(memcpy(dst, src, CHARBUFSIZ) == dst, "memcpy() returns the destination");
    TEST_TRACE(C99 7.21.4.1)
    TEST(memcmp(dst, src, CHARBUFSIZ) == 0, "memcmp() matches");
    dst[0] = 'A';
    TEST(memcmp(dst, src, CHARBUFSIZ) != 0, "memcmp() doesn't match");
    TEST_TRACE(C99 7.21.6.1)
    TEST(memset(dst, 0, CHARBUFSIZ) == dst, "memset() returns the destination");
    TEST(memcmp(dst, zeros, CHARBUFSIZ) == 0, "memcmp() finds all zeros");
    TEST(memset(dst, 1, CHARBUFSIZ) == dst, "memset() returns the destination");
    int flag = 1;
    for (i = 0; i < CHARBUFSIZ; ++i) {
        if (dst[i] != 1) {
            flag = 0;
            break;
        }
    }
    TEST(flag == 1, "memset(1) works");
    TEST_TRACE(C99 7.21.2.2)
    TEST(memmove(dst, src, CHARBUFSIZ) == dst, "memmove() returns the destination");
    TEST(memcmp(dst, src, CHARBUFSIZ) == 0, "memcmp() matches");
    TEST_TRACE(C99 7.21.2.3)
    TEST(memset(dst, 0, CHARBUFSIZ) == dst, "memset() returns the destination");
    TEST(strcpy(dst, src) == dst, "strcpy() returns the destination");
    TEST_TRACE(C99 7.21.4.2)
    TEST(strcmp(dst, src) == 0, "strcmp() matches");
    dst[0] = 'A';
    TEST(strcmp(dst, src) != 0, "strcmp() doesn't match");
    TEST_TRACE(C99 7.21.2.4)
    // Fill dst with non-zero values.
    TEST(memset(dst, 1, CHARBUFSIZ) == dst, "memset() returns the destination");
    TEST(strncpy(dst, src, CHARBUFSIZ) == dst, "strncpy() returns the destination");
    TEST(memcmp(dst, src, CHARBUFSIZ) == 0, "memcmp() matches");
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
    TEST(strxfrm(dst, src, CHARBUFSIZ) >= 0, "strxfrm() returns a positive value");
    TEST(strcmp(dst, src) == 0, "strcmp() matches");
    TEST_TRACE(C99 7.21.5.1)
    TEST(strcpy(dst, "abcdefabcdef") == dst, "strcpy() returns the destination");
    TEST(memchr(dst, 'd', CHARBUFSIZ) == &dst[3], "memchr() finds 'd'");
    TEST(memchr(dst, 'z', CHARBUFSIZ) == NULL, "memchr() does not find 'z'");
    TEST_TRACE(C99 7.21.5.2)
    TEST(strchr(dst, 'd') == &dst[3], "strchr() finds 'd'");
    TEST(strchr(dst, 'z') == NULL, "strchr() does not find 'z'");
    TEST_TRACE(C99 7.21.5.3)
    TEST(strcspn(dst, "zd") == 3, "strcspn() finds 'd'");
    TEST_TRACE(C99 7.21.5.4)
    TEST(strpbrk(dst, "zd") == &dst[3], "strcspn() finds 'd'");
    TEST_TRACE(C99 7.21.5.5)
    TEST(strrchr(dst, 'd') == &dst[6 + 3], "strrchr() finds 'd'");
    TEST(strrchr(dst, 'z') == NULL, "strrchr() does not find 'z'");
    TEST(strrchr(dst, '\0') == &dst[6 + 6], "strrchr() does not find the trailing '\\0'");
    TEST_TRACE(C99 7.21.5.6)
    TEST(strspn(dst, "zcxayb") == 3, "strspn() finds 'd'");
    TEST_TRACE(C99 7.21.5.7)
    TEST(strstr(dst, "def") == &dst[3], "strstr() finds 'def'");
    TEST(strstr(dst, "fed") == NULL, "strstr() does not find 'fed'");
    TEST_TRACE(C99 7.21.5.8)
    // Adapted from C99 7.21.5.8/8
    strcpy(dst, "?a???b,,,#c");
    p = strtok(dst, "?");
    TEST(p != NULL && strcmp(p, "a") == 0, "strtok() found the first token");
    p = strtok(NULL, ",");
    TEST(p != NULL && strcmp(p, "??b") == 0, "strtok() found the second token");
    p = strtok(NULL, "#,");
    TEST(p != NULL && strcmp(p, "c") == 0, "strtok() found the third token");
    p = strtok(NULL, "?");
    TEST(p == NULL, "strtok() returns NULL");
    TEST_TRACE(C99 7.21.6.2)
    TEST(strerror(0) != NULL, "strerror() returns a string");
    TEST_TRACE(C99 7.21.6.3)
    TEST(strlen(src) == 6, "strlen(\"abcdef\") is 6");
END_GROUP
