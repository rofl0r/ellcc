#include <ecc_test.h>
#include <stdio.h>
#include <stdarg.h>

static int myfprintf(FILE *f, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    TEST_TRACE(C99 7.19.6.8)
    int result = vfprintf(f, format, ap);
    va_end(ap);
    return result;
}

static int myfscanf(FILE *f, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    TEST_TRACE(C99 7.19.6.9)
    int result = vfscanf(f, format, ap);
    va_end(ap);
    return result;
}

static int myprintf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    TEST_TRACE(C99 7.19.6.8)
    int result = vprintf(format, ap);
    va_end(ap);
    return result;
}

static int myscanf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    TEST_TRACE(C99 7.19.6.9)
    int result = vscanf(format, ap);
    va_end(ap);
    return result;
}

static int mysnprintf(char *s, size_t n, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    TEST_TRACE(C99 7.19.6.12)
    int result = vsnprintf(s, n, format, ap);
    va_end(ap);
    return result;
}

static int mysprintf(char *s, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    TEST_TRACE(C99 7.19.6.13)
    int result = vsprintf(s, format, ap);
    va_end(ap);
    return result;
}

static int mysscanf(char *s, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    TEST_TRACE(C99 7.19.6.14)
    int result = vsscanf(s, format, ap);
    va_end(ap);
    return result;
}

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
    f = tmpfile();
    TEST(freopen("unlikely filename", "r", f) == NULL, "freopen(unlikely filename) fails as expected");
    fclose(f);
    TEST_TRACE(C99 7.19.5.5)
    setbuf(stderr, NULL);
    TEST_TRACE(C99 7.19.5.6)
    f = tmpfile();
    TEST(setvbuf(f, NULL, _IOFBF, 256) == 0, "setvbuf() succeeds");
    TEST_TRACE(C99 7.19.6.1)
    // See 001format.c for more extensive format checking.
    TEST(fprintf(f, "hello world\n") == 12, "fprintf(hello world\\n) returns 12");
    TEST_TRACE(C99 7.19.9.5)
    rewind(f);
    TEST_TRACE(C99 7.19.6.2)
    TEST(fscanf(f, "%s", buffer) == 1, "fscanf(%%s\\n) returns 1");
    TEST(strcmp(buffer, "hello") == 0, "fscanf succeeds");
    TEST(fscanf(f, "%s", buffer) == 1, "fscanf(%%s\\n) returns 1");
    TEST(strcmp(buffer, "world") == 0, "fscanf succeeds");
    fclose(f);
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
    TEST_TRACE(C99 7.19.6.8)
    f = tmpfile();
    TEST(myfprintf(f, "hello world\n") == 12, "myfprintf(hello world\\n) returns 12");
    rewind(f);
    TEST_TRACE(C99 7.19.6.9)
    TEST(myfscanf(f, "%s", buffer) == 1, "myfscanf(%%s\\n) returns 1");
    TEST(strcmp(buffer, "hello") == 0, "myfscanf succeeds");
    fclose(f);
    TEST_TRACE(C99 7.19.6.10)
    TEST(myprintf("") == 0, "myprintf() does nothing");
    TEST_TRACE(C99 7.19.6.11)
    TEST(myscanf("") == 0, "myscanf() does nothing");
    TEST_TRACE(C99 7.19.6.12)
    memset(buffer, 1, sizeof(buffer));
    TEST(mysnprintf(buffer, sizeof(buffer), "%s", "hello world") == 11, "mysnprintf(hello world) returns 11");
    TEST(strcmp(buffer, "hello world") == 0, "mysnprintf(hello world) succeeds");
    TEST_TRACE(C99 7.19.6.13)
    TEST(mysprintf(buffer, "%s", "hello world") == 11, "mysprintf(hello world) returns 11");
    TEST(strcmp(buffer, "hello world") == 0, "mysprintf(hello world) succeeds");
    TEST_TRACE(C99 7.19.6.14)
    TEST(mysscanf("hello world", "%s", buffer) == 1, "mysscanf(hello world) returns 1");
    TEST(strcmp(buffer, "hello") == 0, "mysscanf(hello world) succeeds");
    TEST_TRACE(C99 7.19.7.1)
    f = tmpfile();
    TEST(fprintf(f, "hello\n") == 6, "fprintf(hello\\n) returns 6");
    rewind(f);
    TEST(fgetc(f) == 'h', "fgetc() returns 'h'");
    TEST(fgetc(f) == 'e', "fgetc() returns 'e'");
    TEST(fgetc(f) == 'l', "fgetc() returns 'l'");
    TEST(fgetc(f) == 'l', "fgetc() returns 'l'");
    TEST(fgetc(f) == 'o', "fgetc() returns 'o'");
    TEST(fgetc(f) == '\n', "fgetc() returns '\\n'");
    TEST(fgetc(f) == EOF, "fgetc() returns EOF");
    rewind(f);
    TEST_TRACE(C99 7.19.7.2)
    TEST(fgets(buffer, sizeof(buffer), f) == buffer, "fgets() returns buffer");
    TEST(strcmp(buffer, "hello\n") == 0, "fgets succeeds");
    TEST(fgets(buffer, sizeof(buffer), f) == NULL, "fgets() returns NULL");
    fclose(f);
    TEST_TRACE(C99 7.19.7.3)
    f = tmpfile();
    TEST(fputc('h', f) == 'h', "fputc(h) returns 'h'");
    TEST(fputc('e', f) == 'e', "fputc(e) returns 'e'");
    TEST(fputc('l', f) == 'l', "fputc(l) returns 'l'");
    TEST(fputc('l', f) == 'l', "fputc(l) returns 'l'");
    TEST(fputc('o', f) == 'o', "fputc(o) returns 'o'");
    TEST(fputc(' ', f) == ' ', "fgetc( ) returns ' '");
    TEST_TRACE(C99 7.19.7.4)
    TEST(fputs("world\n", f) >= 0, "fputs(world\\n) succeeds");
    rewind(f);
    TEST(fgets(buffer, sizeof(buffer), f) == buffer, "fgets() returns buffer");
    TEST(strcmp(buffer, "hello world\n") == 0, "fgets succeeds");
    fclose(f);
    TEST_TRACE(C99 7.19.7.5)
    f = tmpfile();
    TEST(fprintf(f, "hello\n") == 6, "fprintf(hello\\n) returns 6");
    rewind(f);
    TEST(getc(f) == 'h', "fgetc() returns 'h'");
    TEST(getc(f) == 'e', "fgetc() returns 'e'");
    TEST(getc(f) == 'l', "fgetc() returns 'l'");
    TEST(getc(f) == 'l', "fgetc() returns 'l'");
    TEST(getc(f) == 'o', "fgetc() returns 'o'");
    TEST(getc(f) == '\n', "fgetc() returns '\\n'");
    TEST(getc(f) == EOF, "fgetc() returns EOF");
    fclose(f);
    TEST_TRACE(C99 7.19.7.11)
    TEST(ungetc('A', stdin) == 'A', "ungetc succeeds");
    TEST_TRACE(C99 7.19.7.6)
    TEST(getchar()  == 'A', "getchar succeeds");
    TEST_TRACE(C99 7.19.7.7)
    TEST(ungetc('\n', stdin) == '\n', "ungetc succeeds");
    TEST(gets(buffer) == buffer, "gets returns buffer");
    TEST(strcmp(buffer, "") == 0, "gets succeeds");
    TEST_TRACE(C99 7.19.7.8)
    f = tmpfile();
    TEST(putc('h', f) == 'h', "fputc(h) returns 'h'");
    TEST(putc('e', f) == 'e', "fputc(e) returns 'e'");
    TEST(putc('l', f) == 'l', "fputc(l) returns 'l'");
    TEST(putc('l', f) == 'l', "fputc(l) returns 'l'");
    TEST(putc('o', f) == 'o', "fputc(o) returns 'o'");
    TEST(putc('\n', f) == '\n', "fgetc( ) returns '\\n'");
    rewind(f);
    TEST(fgets(buffer, sizeof(buffer), f) == buffer, "fgets() returns buffer");
    fclose(f);
    TEST(strcmp(buffer, "hello\n") == 0, "fgets succeeds");
    TEST_TRACE(C99 7.19.7.9)
    TEST(putchar('#') == '#' && putchar('\n') == '\n', "putchar succeeds");
    TEST_TRACE(C99 7.19.7.10)
    TEST(puts("# hello world") >= 0, "puts succeeds");
    TEST_TRACE(C99 7.19.8.2)
    f = tmpfile();
    memset(buffer, 0xAA, sizeof(buffer));
    TEST(fwrite(buffer, sizeof(buffer), 1, f) == 1, "fwrite returns 1");
    TEST_TRACE(C99 7.19.8.1)
    rewind(f);
    char buffer2[sizeof(buffer)];
    TEST(fread(buffer2, sizeof(buffer), 2, f) == 1, "fread returns 1");
    TEST(memcmp(buffer, buffer2, sizeof(buffer)) == 0, "fwrite and fread succeed");
    rewind(f);
    TEST_TRACE(C99 7.19.9.1)
    TEST(fgetpos(f, &fpos) == 0, "fgetpos succeeds");
    TEST_TRACE(C99 7.19.9.4)
    long int li = ftell(f);
    TEST(li != -1L, "ftell succeeds");
    TEST(fread(buffer2, sizeof(buffer), 2, f) == 1, "fread returns 1");
    TEST(memcmp(buffer, buffer2, sizeof(buffer)) == 0, "fwrite and fread succeed");
    TEST_TRACE(C99 7.19.9.2)
    TEST(fseek(f, li, SEEK_SET) == 0, "fseek returns 0");
    TEST(fread(buffer2, sizeof(buffer), 2, f) == 1, "fread returns 1");
    TEST(memcmp(buffer, buffer2, sizeof(buffer)) == 0, "fread succeeds");
    TEST_TRACE(C99 7.19.9.3)
    memset(buffer2, 0, sizeof(buffer));
    TEST(fsetpos(f, &fpos) == 0, "fsetpos returns 0");
    TEST(fread(buffer2, sizeof(buffer), 2, f) == 1, "fread returns 1");
    TEST(memcmp(buffer, buffer2, sizeof(buffer)) == 0, "fread succeeds");
END_GROUP

