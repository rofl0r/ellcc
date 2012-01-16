#include <ecc_test.h>
#include <wchar.h>
#include <stdarg.h>

static int myfwprintf(FILE *f, const wchar_t *format, ...)
{
    va_list ap;
    va_start(ap, format);
    TEST_TRACE(C99 7.24.2.5)
    int result = vfwprintf(f, format, ap);
    va_end(ap);
    return result;
}

static int myfwscanf(FILE *f, const wchar_t *format, ...)
{
    va_list ap;
    va_start(ap, format);
    TEST_TRACE(C99 7.24.2.6)
    int result = vfwscanf(f, format, ap);
    va_end(ap);
    return result;
}

static int mywprintf(const wchar_t *format, ...)
{
    va_list ap;
    va_start(ap, format);
    TEST_TRACE(C99 7.24.2.9)
    int result = vwprintf(format, ap);
    va_end(ap);
    return result;
}

static int mywscanf(const wchar_t *format, ...)
{
    va_list ap;
    va_start(ap, format);
    TEST_TRACE(C99 7.24.2.10)
    int result = vwscanf(format, ap);
    va_end(ap);
    return result;
}

static int myswprintf(wchar_t *s, size_t n, const wchar_t *format, ...)
{
    va_list ap;
    va_start(ap, format);
    TEST_TRACE(C99 7.24.2.7)
    int result = vswprintf(s, n, format, ap);
    va_end(ap);
    return result;
}

static int myswscanf(wchar_t *s, const wchar_t *format, ...)
{
    va_list ap;
    va_start(ap, format);
    TEST_TRACE(C99 7.24.2.8)
    int result = vswscanf(s, format, ap);
    va_end(ap);
    return result;
}

TEST_GROUP(Wchar)
    FILE *f;
    TEST_TRACE(C99 7.24/2)
    wchar_t wchar;
    size_t size;
    mbstate_t mbstate;
    wint_t wint;
    struct tm *tm;
    wchar_t buffer[100];
    TEST_TRACE(C99 7.24/3)
    f = NULL;
    wint = WCHAR_MIN;
    wint = WCHAR_MAX;
    wint = WEOF;
    TEST_TRACE(C99 7.24.3.5)
    f = tmpfile();
    TEST(fwide(f, 1) > 0, "fwide() created a wide stream");
    TEST_TRACE(C99 7.24.2.1)
    TEST(fwprintf(f, L"hello world\n") == 12, "fwprintf(hello world\\n) returns 12");
    TEST_TRACE(C99 7.24.2.2)
    rewind(f);
    TEST(fwscanf(f, L"%ls", buffer) == 1, "fwscanf(%%s\\n) returns 1");
    TEST_TRACE(C99 7.24.4.4.1)
    TEST(wcscmp(buffer, L"hello") == 0, "fwscanf succeeds");
    TEST(fwscanf(f, L"%ls", buffer) == 1, "fwscanf(%%s\\n) returns 1");
    TEST(wcscmp(buffer, L"world") == 0, "fwscanf succeeds");
    fclose(f);
    TEST_TRACE(C99 7.24.2.3)
    memset(buffer, 1, sizeof(buffer));
    TEST(swprintf(buffer, 0, L"%ls", L"hello world") < 0, "swprintf() with 0");
    TEST(buffer[0] == 0x01010101, "swprintf() did not change the buffer");
    TEST(swprintf(buffer, 1, L"%ls", L"hello world") < 0, "swprintf() with 1");
    TEST(buffer[0] == L'\0', "swprintf() added a trailing nul");
    TEST(swprintf(buffer, 100, L"%ls", L"hello world") == 11, "swprintf(hello world)");
    TEST(wcscmp(buffer, L"hello world") == 0, "swprintf(hello world) succeeds");
    TEST_TRACE(C99 7.24.2.4)
    TEST(swscanf(L"hello world", L"%ls", buffer) == 1, "swscanf(hello world)");
    TEST(wcscmp(buffer, L"hello") == 0, "swscanf(hello world) succeeds");
    TEST_TRACE(C99 7.24.2.5)
    f = tmpfile();
    TEST(fwide(f, 1) > 0, "fwide() created a wide stream");
    TEST(myfwprintf(f, L"hello world\n") == 12, "myfwprintf(hello world\\n) returns 12");
    rewind(f);
    TEST_TRACE(C99 7.24.2.6)
    TEST(myfwscanf(f, L"%ls", buffer) == 1, "myfwscanf(%%ls\\n) returns 1");
    TEST(wcscmp(buffer, L"hello") == 0, "myfwscanf succeeds");
    fclose(f);
    TEST_TRACE(C99 7.24.2.7)
    memset(buffer, 1, sizeof(buffer));
    TEST(myswprintf(buffer, 100, L"%ls", L"hello world") == 11, "myswprintf(hello world) returns 11");
    TEST(wcscmp(buffer, L"hello world") == 0, "myswprintf(hello world) succeeds");
    TEST_TRACE(C99 7.24.2.8)
    TEST(myswscanf(L"hello world", L"%ls", buffer) == 1, "myswscanf(hello world) returns 1");
    TEST(wcscmp(buffer, L"hello") == 0, "myswscanf(hello world) succeeds");
    TEST_TRACE(C99 7.24.2.9)
    TEST(mywprintf(L"") == 0, "mywprintf() does nothing");
    TEST_TRACE(C99 7.24.2.10)
    TEST(mywscanf(L"") == 0, "mywscanf() does nothing");
    TEST_TRACE(C99 7.24.2.11)
    TEST(wprintf(L"") == 0, "wprintf() does nothing");
    TEST_TRACE(C99 7.24.2.12)
    TEST(wscanf(L"") == 0, "wscanf() does nothing");
END_GROUP
