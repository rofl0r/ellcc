#include <ecc_test.h>
#include <wchar.h>
#include <stdarg.h>
#include <time.h>

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
    int i;
    float fl;
    double d;
    long double ld;
    long l;
    unsigned long ul;
    long long ll;
    unsigned long long ull;
    FILE *f;
    const struct tm ctm = {
        .tm_sec = 59,
        .tm_min = 9,
        .tm_hour = 13,
        .tm_mday = 14,
        .tm_mon = 0,
        .tm_year = 2012 - 1900,
        .tm_wday = 6,
        .tm_yday = 13,
        .tm_isdst = 0,
    };
    TEST_TRACE(C99 7.24/2)
    wchar_t wchar;
    size_t size;
    mbstate_t mbstate;
    wint_t wint;
    struct tm *tm;
    wchar_t *p;
#define WCHARBUFSIZ 100
    static const wchar_t src[WCHARBUFSIZ] = L"abcdef";
    static const wchar_t zeros[WCHARBUFSIZ];
    wchar_t dst[WCHARBUFSIZ];
    wchar_t buffer[WCHARBUFSIZ];
    char mbc[MB_CUR_MAX];
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
    TEST(swprintf(buffer, WCHARBUFSIZ, L"%ls", L"hello world") == 11, "swprintf(hello world)");
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
    TEST(myswprintf(buffer, WCHARBUFSIZ, L"%ls", L"hello world") == 11, "myswprintf(hello world) returns 11");
    TEST(wcscmp(buffer, L"hello world") == 0, "myswprintf(hello world) succeeds");
    TEST_TRACE(C99 7.24.2.8)
    TEST(myswscanf(L"hello world", L"%ls", buffer) == 1, "myswscanf(hello world) returns 1");
    TEST(wcscmp(buffer, L"hello") == 0, "myswscanf(hello world) succeeds");
    TEST_TRACE(C99 7.24.2.9)
    TEST_EXCLUDE(HOST, "http://ellcc.org/bugzilla/show_bug.cgi?id=37")
        TEST(mywprintf(L"") == 0, "mywprintf() does nothing");
    TEST_TRACE(C99 7.24.2.10)
    TEST(mywscanf(L"") == 0, "mywscanf() does nothing");
    TEST_TRACE(C99 7.24.2.11)
    TEST_EXCLUDE(HOST, "http://ellcc.org/bugzilla/show_bug.cgi?id=37")
        TEST(wprintf(L"") == 0, "wprintf() does nothing");
    TEST_TRACE(C99 7.24.2.12)
    TEST(wscanf(L"") == 0, "wscanf() does nothing");
    TEST_TRACE(C99 7.24.3.1)
    f = tmpfile();
    TEST(fwprintf(f, L"hello\n") == 6, "fwprintf(hello\\n) returns 6");
    rewind(f);
    TEST(fgetwc(f) == L'h', "fgetwc() returns 'h'");
    TEST(fgetwc(f) == L'e', "fgetwc() returns 'e'");
    TEST(fgetwc(f) == L'l', "fgetwc() returns 'l'");
    TEST(fgetwc(f) == L'l', "fgetwc() returns 'l'");
    TEST(fgetwc(f) == L'o', "fgetwc() returns 'o'");
    TEST(fgetwc(f) == L'\n', "fgetwc() returns '\\n'");
    TEST(fgetwc(f) == WEOF, "fgetwc() returns WEOF");
    TEST_TRACE(C99 7.24.3.2)
    rewind(f);
    TEST(fgetws(buffer, WCHARBUFSIZ, f) == buffer, "fgetws() returns buffer");
    TEST(wcscmp(buffer, L"hello\n") == 0, "fgetws succeeds");
    TEST(fgetws(buffer, WCHARBUFSIZ, f) == NULL, "fgetws() returns NULL");
    fclose(f);
    TEST_TRACE(C99 7.24.3.3)
    f = tmpfile();
    TEST(fputwc(L'h', f) == L'h', "fwputc(h) returns 'h'");
    TEST(fputwc(L'e', f) == L'e', "fwputc(e) returns 'e'");
    TEST(fputwc(L'l', f) == L'l', "fwputc(l) returns 'l'");
    TEST(fputwc(L'l', f) == L'l', "fwputc(l) returns 'l'");
    TEST(fputwc(L'o', f) == L'o', "fwputc(o) returns 'o'");
    TEST(fputwc(L' ', f) == L' ', "fwgetc( ) returns ' '");
    TEST_TRACE(C99 7.24.3.4)
    TEST(fputws(L"world\n", f) >= 0, "fputws(world\\n) succeeds");
    rewind(f);
    TEST(fgetws(buffer, WCHARBUFSIZ, f) == buffer, "fwgets() returns buffer");
    TEST(wcscmp(buffer, L"hello world\n") == 0, "fgets succeeds");
    fclose(f);
    TEST_TRACE(C99 7.24.3.6)
    f = tmpfile();
    TEST(fwprintf(f, L"hello\n") == 6, "fwprintf(hello\\n) returns 6");
    rewind(f);
    TEST(getwc(f) == L'h', "getwc() returns 'h'");
    TEST(getwc(f) == L'e', "getwc() returns 'e'");
    TEST(getwc(f) == L'l', "getwc() returns 'l'");
    TEST(getwc(f) == L'l', "getwc() returns 'l'");
    TEST(getwc(f) == L'o', "getwc() returns 'o'");
    TEST(getwc(f) == L'\n', "getwc() returns '\\n'");
    TEST(getwc(f) == WEOF, "getwc() returns EOF");
    fclose(f);
    TEST_TRACE(C99 7.24.3.11)
    TEST(ungetwc(L'A', stdin) == L'A', "ungetwc succeeds");
    TEST_TRACE(C99 7.24.3.7)
    TEST(getwchar()  == L'A', "getchar succeeds");
    TEST_TRACE(C99 7.24.3.8)
    f = tmpfile();
    TEST(putwc('h', f) == 'h', "putwc(h) returns 'h'");
    TEST(putwc('e', f) == 'e', "putwc(e) returns 'e'");
    TEST(putwc('l', f) == 'l', "putwc(l) returns 'l'");
    TEST(putwc('l', f) == 'l', "putwc(l) returns 'l'");
    TEST(putwc('o', f) == 'o', "putwc(o) returns 'o'");
    TEST(putwc('\n', f) == '\n', "putwc( ) returns '\\n'");
    rewind(f);
    TEST(fgetws(buffer, sizeof(buffer), f) == buffer, "fgetws() returns buffer");
    fclose(f);
    TEST_TRACE(C99 7.24.3.9)
    TEST(putwchar(L'#') == L'#' && putwchar(L'\n') == L'\n', "putwchar succeeds");
    TEST_TRACE(C99 7.24.4.1.1)
    d = wcstod(L"1.0", &p);
    TEST(d == 1.0 && *p == L'\0', "wcstod(1.0) == 1.0");
    fl = wcstof(L"1.0", &p);
    TEST(fl == 1.0F && *p == L'\0', "wcstof(1.0) == 1.0F");
    ld = wcstold(L"1.0", &p);
    TEST(ld == 1.0L && *p == L'\0', "wcstold(1.0) == 1.0L");
    TEST_TRACE(C99 7.24.4.1.2)
    l = wcstol(L"12345678", &p, 0);
    TEST(l == 12345678L && *p == '\0', "wcstol(12345678L) == 12345678L");
    ul = wcstoul(L"12345678", &p, 0);
    TEST(ul == 12345678UL && *p == '\0', "wcstoul(12345678UL) == 12345678UL");
    ll = wcstoll(L"12345678901234", &p, 0);
    TEST(ll == 12345678901234LL && *p == '\0', "wcstoll(12345678901234LL) == 12345678901234LL");
    ull = wcstoull(L"12345678901234", &p, 0);
    TEST(ull == 12345678901234LL && *p == '\0', "wcstoull(12345678901234LL) == 12345678901234LL");
    TEST_TRACE(C99 7.24.4.2.1)
    TEST(wcscpy(dst, src) == dst, "wcscpy() returns the destination");
    TEST_TRACE(C99 7.24.4.4.1)
    TEST(wcscmp(dst, src) == 0, "wcscmp() matches");
    dst[0] = L'A';
    TEST(wcscmp(dst, src) != 0, "wcscmp() doesn't match");
    TEST_TRACE(C99 7.24.4.6.2)
    // Fill dst with non-zero values.
    TEST(wmemset(dst, 1, WCHARBUFSIZ) == dst, "wmemset() returns the destination");
    TEST_TRACE(C99 7.24.4.2.2)
    TEST(wcsncpy(dst, src, WCHARBUFSIZ) == dst, "wcsncpy() returns the destination");
    TEST_TRACE(C99 7.24.4.4.5)
    TEST(wmemcmp(dst, src, WCHARBUFSIZ) == 0, "wmemcmp() matches");
    TEST_TRACE(C99 7.24.4.2.3)
    // Fill dst with non-zero values.
    TEST(wmemset(dst, 1, WCHARBUFSIZ) == dst, "wmemset() returns the destination");
    TEST(wmemcpy(dst, src, WCHARBUFSIZ) == dst, "wmemcpy() returns the destination");
    TEST(wmemcmp(dst, src, WCHARBUFSIZ) == 0, "wmemcmp() matches");
    TEST_TRACE(C99 7.24.4.2.4)
    // Fill dst with non-zero values.
    TEST(wmemset(dst, 1, WCHARBUFSIZ) == dst, "wmemset() returns the destination");
    TEST(wmemmove(dst, src, WCHARBUFSIZ) == dst, "wmemmove() returns the destination");
    TEST(wmemcmp(dst, src, WCHARBUFSIZ) == 0, "wmemcmp() matches");
    TEST_TRACE(C99 7.24.4.3.1)
    TEST(wcscat(dst, src) == dst, "wcscat() returns the destination");
    TEST(wcscmp(dst, L"abcdefabcdef") == 0, "wcscmp() matches");
    TEST_TRACE(C99 7.24.4.3.2)
    TEST(wcscpy(dst, src) == dst, "wcscpy() returns the destination");
    TEST(wcsncat(dst, src, 4) == dst, "wcsncat() returns the destination");
    TEST(wcscmp(dst, L"abcdefabcd") == 0, "wcscmp() matches");
    TEST_TRACE(C99 7.24.4.4.2)
    TEST(wcscpy(dst, src) == dst, "wcscpy() returns the destination");
    TEST(wcscoll(dst, src) == 0, "wcscoll() matches");
    TEST_TRACE(C99 7.24.4.4.3)
    TEST(wcsncmp(dst, L"abcdzzz", 4) == 0, "wcsncmp() matches");
    TEST_TRACE(C99 7.24.4.4.4)
    TEST(wcsxfrm(dst, src, WCHARBUFSIZ) >= 0, "wcsxfrm() returns a positive value");
    TEST(wcscmp(dst, src) == 0, "wcscmp() matches");
    TEST_TRACE(C99 7.24.4.5.1)
    TEST(wcscpy(dst, L"abcdefabcdef") == dst, "wcscpy() returns the destination");
    TEST(wcschr(dst, L'd') == &dst[3], "wcschr() finds 'd'");
    TEST(wcschr(dst, L'z') == NULL, "wcschr() does not find 'z'");
    TEST_TRACE(C99 7.24.4.5.2)
    TEST(wcscspn(dst, L"zd") == 3, "wcscspn() finds 'd'");
    TEST_TRACE(C99 7.24.4.5.3)
    TEST(wcspbrk(dst, L"zd") == &dst[3], "wcscspn() finds 'd'");
    TEST_TRACE(C99 7.24.4.5.4)
    TEST(wcsrchr(dst, L'd') == &dst[6 + 3], "wcsrchr() finds 'd'");
    TEST(wcsrchr(dst, L'z') == NULL, "wcsrchr() does not find 'z'");
    TEST_TRACE(C99 7.24.4.5.5)
    TEST(wcsspn(dst, L"zcxayb") == 3, "wcsspn() finds 'd'");
    TEST_TRACE(C99 7.24.4.5.6)
    TEST(wcsstr(dst, L"def") == &dst[3], "wcsstr() finds 'def'");
    TEST(wcsstr(dst, L"fed") == NULL, "wcsstr() does not find 'fed'");
    TEST_TRACE(C99 7.24.4.5.7)
    // Adapted from C99 7.21.5.8/8
    wcscpy(dst, L"?a???b,,,#c");
    wchar_t *ptr;
    p = wcstok(dst, L"?", &ptr);
    TEST(p != NULL && wcscmp(p, L"a") == 0, "wcstok() found the first token");
    p = wcstok(NULL, L",", &ptr);
    TEST(p != NULL && wcscmp(p, L"??b") == 0, "wcstok() found the second token");
    p = wcstok(NULL, L"#,", &ptr);
    TEST(p != NULL && wcscmp(p, L"c") == 0, "wcstok() found the third token");
    p = wcstok(NULL, L"?", &ptr);
    TEST(p == NULL, "wcstok() returns NULL");
    TEST_TRACE(C99 7.24.4.5.8)
    TEST(wcscpy(dst, L"abcdefabcdef") == dst, "wcscpy() returns the destination");
    TEST(wmemchr(dst, L'd', WCHARBUFSIZ) == &dst[3], "wmemchr() finds 'd'");
    TEST(wmemchr(dst, L'z', WCHARBUFSIZ) == NULL, "wmemchr() does not find 'z'");
    TEST_TRACE(C99 7.24.4.6.1)
    TEST(wcslen(src) == 6, "wcslen(\"abcdef\") is 6");
    TEST_TRACE(C99 7.24.4.6.2)
    TEST(wmemset(dst, 0, WCHARBUFSIZ) == dst, "wmemset() returns the destination");
    TEST(wmemcmp(dst, zeros, WCHARBUFSIZ) == 0, "wmemcmp() finds all zeros");
    TEST(wmemset(dst, 1, WCHARBUFSIZ) == dst, "wmemset() returns the destination");
    int flag = 1;
    for (i = 0; i < WCHARBUFSIZ; ++i) {
        if (dst[i] != 1) {
            flag = 0;
            break;
        }
    }
    TEST_TRACE(C99 7.24.5.1)
    TEST_RESOLVED(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=33")
    TEST_EXCLUDE(PPC64, "http://ellcc.org/bugzilla/show_bug.cgi?id=34")
    TEST_EXCLUDE(PPC, "http://ellcc.org/bugzilla/show_bug.cgi?id=35")
    {
        size = wcsftime(buffer, WCHARBUFSIZ, L"format", &ctm);
        TEST(size == 6, "wcsftime() returns the number of bytes written");
        TEST(wcscmp(buffer, L"format") == 0, "wcsftime() copies non-format characters");
    }
    TEST_TRACE(C99 7.24.6.1.1)
    TEST(btowc('A') == L'A', "btowc('A') == L'A'");
    TEST(btowc(EOF) == WEOF, "btowc(EOF) == WEOF");
    TEST_TRACE(C99 7.24.6.1.2)
    TEST(wctob(L'A') == 'A', "wctob(L'A') == 'A'");
    TEST_TRACE(C99 7.24.6.2.1)
    TEST(mbsinit(NULL) != 0, "mbsinit(NULL) != 0");
    TEST_TRACE(C99 7.24.6.3.1)
    memset(&mbstate, 0, sizeof(mbstate));
    TEST(mbrlen("", 1, &mbstate) == 0, "mbrlen(\"\") returns 0");
    TEST(mbrlen("A", 1, &mbstate) == 1, "mbrlen(\"A\") returns 1");
    TEST_TRACE(C99 7.24.6.3.2)
    TEST(mbrtowc(NULL, "", 1, &mbstate) == 0, "mbrtowc(\"\") returns 0");
    TEST(mbrtowc(NULL, "A", 1, &mbstate) == 1, "mbrtowc(\"A\") returns 1");
    TEST(mbrtowc(&wchar, "", 1, &mbstate) == 0, "mbrtowc(\"\") returns 0");
    TEST(wchar == L'\0', "the converted value is '\\0'");
    TEST(mbrtowc(&wchar, "A", 1, &mbstate) == 1, "mbrtowc(\"A\") returns 1");
    TEST(wchar == L'A', "the converted value is 'A'");
    TEST_TRACE(C99 7.24.6.3.3)
    TEST(wcrtomb(mbc, L'A', &mbstate) == 1, "wcrtomb(\"A\") returns 1");
    TEST_EXCLUDE(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=38")
        TEST(mbc[0] == 'A', "the multibyte character is 'A'");
    TEST_TRACE(C99 7.24.6.4.1)
    const char *mbsrc = "a";
    TEST(mbsrtowcs(buffer, &mbsrc, WCHARBUFSIZ, &mbstate) == 1, "\"a\" is one multibyte character");
    TEST(buffer[0] == L'a', "'a' is L'a'");
    TEST_TRACE(C99 7.24.6.4.2)
    const wchar_t *wcsrc = L"A";
    TEST(wcsrtombs(mbc, &wcsrc, MB_CUR_MAX, &mbstate) == 1, "\"a\" is one multibyte character");
    TEST_EXCLUDE(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=38")
        TEST(mbc[0] == 'A', "L'A' is 'A'");
END_GROUP
