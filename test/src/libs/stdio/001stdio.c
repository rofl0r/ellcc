#include <ecc_test.h>
#include <stdio.h>

TEST_GROUP(Stdio)
    TEST_TRACE(C99 7.19.1/2)
    size_t size;
    FILE *f;
    fpos_t fpos;
    int i;
    char *p;
    TEST_TRACE(C99 7.19.1/3)
    p = NULL;
    TEST(_IOFBF != _IOLBF && _IOFBF != _IONBF,
         "_IOFBF has a distinct value");
    TEST(_IOLBF != _IONBF,
         "_IOLBF has a distinct value");
    TEST(BUFSIZ >= 256, "BUFSIZE is at least 256");
    TEST(EOF < 0, "EOF is a negative value");
    TEST(FOPEN_MAX >= 8, "FOPEN_MAX is at least 8");
    i = FILENAME_MAX;
    i = L_tmpnam;
    TEST(SEEK_CUR != SEEK_END && SEEK_CUR != SEEK_SET,
         "SEEK_CUR has a distinct value");
    TEST(SEEK_END != SEEK_SET,
         "SEEK_END has a distinct value");
    TEST(TMP_MAX >= 25, "TMP_MAX is at least 25");
    f = stderr;
    f = stdin;
    f = stdout;
    TEST_TRACE(C99 7.19.4.1)
    TEST(remove("unlikely filename") != 0, "remove() fails as expected");
    TEST_TRACE(C99 7.19.4.2)
    TEST(rename("unlikely filename", "very unlikely filename") != 0, "rename() fails as expected");
    TEST_TRACE(C99 7.19.4.3)
    f = tmpfile();
    TEST(f != NULL, "Have a file pointer from tmpfile()");
END_GROUP

