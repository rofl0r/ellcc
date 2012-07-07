#include <ecc_test.h>
#include <stdio.h>
#include <string.h>

TEST_GROUP(Format)
    TEST_TRACE(C99 7.19.6)
    char buffer[10240];

    int length = sprintf(buffer, "hello\n");
    TEST(strcmp(buffer, "hello\n") == 0, "A buffer can be filled via a format");
    TEST(length == 6, "The formatter returns the length of the formatted data");

    sprintf(buffer, "%%");
    TEST(strcmp(buffer, "%") == 0, "Format %%%% gives %s", buffer);

    // Check formatting.
#define chk(fmt, out, ...)                                                       \
    length = sprintf(buffer, fmt, __VA_ARGS__);                              \
    TEST(strcmp(buffer, out) == 0, "Format \"%s\" with (" #__VA_ARGS__ ") gives \"%s\"", fmt, buffer)

    chk("%d", "10", 10);
    chk("% d", " 10", 10);
    chk("% d", "-10", -10);
    chk("%-3d", "10 ", 10);
    chk("%+d", "+10", 10);
    chk("%+d", "-10", -10);
    chk("%03d", "010", 10);
    chk("%03d", "-10", -10);
    chk("%*d", " 10", 3, 10);
    chk("%ld", "10", 10l);
    chk("%lld", "10", 10ll);
    chk("%lld", "-10", -10ll);
    chk("%hd", "10", (short)10);
    chk("%u", "10", 10);
    chk("%lu", "10", 10l);
    chk("%llu", "10", 10ll);
    TEST_EXCLUDE(ARM, "http://ellcc.org/bugzilla/show_bug.cgi?id=52") {
        chk("%e", "1.000000e+01", 10.0);
        chk("%E", "1.000000E+01", 10.0);
        chk("%e", "1.000000e-02", 0.010);
        chk("%g", "1e+50", 1e50);
        chk("%e", "1.000000e+50", 1e50);
        chk("%e", "1.000000e+150", 1e150);
        chk("%f", "10.000000", 10.0);
        chk("%g", "10", 10.0);
        chk("%g", "-10", -10.0);
        chk("%g", "1e+09", 1000000000.0);
        chk("%G", "1E+09", 1000000000.0);
        chk("%g", "0.1", 0.1);
        chk("%g", "10", 9.99999999);
        chk("%10.9g", "9.99999999", 9.99999999);
        chk("%*.9g", "9.99999999", 10, 9.99999999);
        chk("%10.*g", "9.99999999", 9, 9.99999999);
        chk("%*.g", "     1e+01", 10, 9.99999999);
        chk("%g", "0.01", 0.01);
    }
    chk("%x", "a", 10);
    chk("%lx", "a", 10l);
    chk("%llx", "a", 10ll);
    chk("%#x", "0xa", 10);
    chk("%#X", "0XA", 10);
    chk("%X", "A", 10);
    chk("%o", "12", 10);
    chk("%#o", "012", 10);
    chk("%#o", "0", 0);
    chk("%lo", "12", 10l);
    chk("%llo", "12", 10ll);

    {
    int len;
    chk("%d%n", "10", 10, &len);
    TEST(len == 2, "Check format length");
    }

    {
    long len;
    chk("%d%ln", "10", 10, &len);
    TEST(len == 2, "Check format length gives %ld", len);
    }

    {
    long long len;
    chk("%d%lln", "10", 10, &len);
    TEST(len == 2, "Check format length");
    }

    {
    short len;
    chk("%d%hn", "10", 10, &len);
    TEST(len == 2, "Check format length");
    }

    static const char string[] = {
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
        "1234567890"
    };
    sprintf(buffer, "%s", string);
    TEST(strcmp(buffer, string) == 0, "Format of a long string");
END_GROUP
