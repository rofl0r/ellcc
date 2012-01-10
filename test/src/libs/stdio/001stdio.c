#include <ecc_test.h>
#include <stdio.h>

TEST_GROUP(Stdio)
    TEST_TRACE(C99 7.19.1/2)
    size_t size;
    FILE *f;
    fpos_t fpos;
    int i;
    char *p;
    char buffer[100];
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
    TEST(remove("unlikely filename") != 0, "remove(unlikely filename) fails as expected");
    TEST_TRACE(C99 7.19.4.2)
    TEST(rename("unlikely filename", "very unlikely filename") != 0, "rename() fails as expected");
    TEST_TRACE(C99 7.19.4.3)
    f = tmpfile();
    TEST(f != NULL, "Have a file pointer from tmpfile()");
    TEST_TRACE(C99 7.19.5.1)
    TEST(fclose(f) == 0, "fclose() is successful");
    TEST_TRACE(C99 7.19.5.2)
    TEST(fflush(f) == EOF, "fflush() is unsuccessful");
    TEST_TRACE(C99 7.19.5.3)
    f = fopen("tmp", "w");
    TEST(f != NULL, "fopen(tmp) is successful");
    TEST(fclose(f) == 0, "fclose() is successful");
    TEST(remove("tmp") == 0, "remove(tmp) works as expected");
    TEST(fopen("unlikely filename", "r") == NULL, "fopen(unlikely filename) fails as expected");
    TEST_TRACE(C99 7.19.5.4)
    TEST(freopen("unlikely filename", "r", stdin) == NULL, "freopen(unlikely filename) fails as expected");
    TEST_TRACE(C99 7.19.5.5)
    setbuf(stderr, NULL);
    TEST_TRACE(C99 7.19.5.6)
    f = tmpfile();
    TEST(setvbuf(f, NULL, _IOFBF, 256) == 0, "setvbuf() succeeds");
    TEST_TRACE(C99 7.19.6.1)
    // See 001format.c for more extensive format checking.
    TEST(fprintf(f, "hello world\n") == 12, "fprintf(hello world\n) returns 12");
    TEST_TRACE(C99 7.19.9.5)
    rewind(f);
    TEST_TRACE(C99 7.19.6.2)
    TEST(fscanf(f, "%s", buffer) == 1, "fscanf(%%s\\n) returns 1");
    TEST(strcmp(buffer, "hello") == 0, "fscanf succeeded");
    TEST(fscanf(f, "%s", buffer) == 1, "fscanf(%%s\\n) returns 1");
    TEST(strcmp(buffer, "world") == 0, "fscanf succeeded");
    TEST_TRACE(C99 7.19.6.3)
    TEST(printf("") == 0, "printf() does nothing");
    TEST_TRACE(C99 7.19.6.4)
    TEST(scanf("") == 0, "scanf() does nothing");
    TEST_TRACE(C99 7.19.6.5)
    memset(buffer, 1, sizeof(buffer));
    TEST(snprintf(buffer, 0, "%s", "hello world") == 11, "snprintf() with 0");
    TEST(buffer[0] == 1, "snprintf() did not change the buffer");
    TEST(snprintf(buffer, 1, "%s", "hello world") == 11, "snprintf() with 1");
    TEST(buffer[0] == 0, "snprintf() added a trailing nul");
    TEST(snprintf(buffer, sizeof(buffer), "%s", "hello world") == 11, "snprintf(hello world)");
    TEST(strcmp(buffer, "hello world") == 0, "snprintf(hello world) succeeds");
    TEST_TRACE(C99 7.19.6.6)
    TEST(sprintf(buffer, "%s", "hello world") == 11, "sprintf(hello world)");
    TEST(strcmp(buffer, "hello world") == 0, "sprintf(hello world) succeeds");
    TEST_TRACE(C99 7.19.6.7)
    TEST(sscanf("hello world", "%s", buffer) == 1, "sscanf(hello world)");
    TEST(strcmp(buffer, "hello") == 0, "sscanf(hello world) succeeds");
END_GROUP

