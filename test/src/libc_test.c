/** @file
 * This file is the C unit test program for the libecc library.
 *
 * @author Richard Pennington
 * @date Sun Jan  9 09:56:06 CST 2011
 */

#include <string.h>
#include "ecc_test.h"

int main(int argc, char** argv)
{
    TEST_INIT(1);
    TEST_CATEGORY(General);
    // General tests.
    TEST(argc == 1, "This program was started with no arguments");
    TEST(argv[0] != NULL, "There is an argv[0] argument");
    TEST(argv[1] == NULL, "There are no other arguments");

    TEST_CATEGORY(Format);
    {
        char buffer[10240];

        int length = sprintf(buffer, "hello\n");
        TEST(strcmp(buffer, "hello\n") == 0, "A buffer can be filled via a format");
        TEST(length == 6, "The formatter returns the length of the formatted data");

        sprintf(buffer, "%d", 10);
        TEST(strcmp(buffer, "10") == 0, "Format %%d of 10 gives %s", buffer);

        sprintf(buffer, "% d", 10);
        TEST(strcmp(buffer, " 10") == 0, "Format %% d of 10 gives %s", buffer);

        sprintf(buffer, "% d", -10);
        TEST(strcmp(buffer, "-10") == 0, "Format %% d of -10 gives %s", buffer);

        sprintf(buffer, "%-3d", 10);
        TEST(strcmp(buffer, "10 ") == 0, "Format %%-3d of 10 gives %s", buffer);

        sprintf(buffer, "%+d", 10);
        TEST(strcmp(buffer, "+10") == 0, "Format %%+d of 10 gives %s", buffer);

        sprintf(buffer, "%+d", -10);
        TEST(strcmp(buffer, "-10") == 0, "Format %%+d of -10 gives %s", buffer);

        sprintf(buffer, "%%");
        TEST(strcmp(buffer, "%") == 0, "Format %%%% gives %s", buffer);

        sprintf(buffer, "%03d", 10);
        TEST(strcmp(buffer, "010") == 0, "Format %%03d of 10 gives %s", buffer);

        sprintf(buffer, "%03d", -10);
        TEST(strcmp(buffer, "-10") == 0, "Format %%03d of -10 gives %s", buffer);

        sprintf(buffer, "%*d", 3, 10);
        TEST(strcmp(buffer, " 10") == 0, "Format %%3d of 10 gives %s", buffer);

        sprintf(buffer, "%ld", 10l);
        TEST(strcmp(buffer, "10") == 0, "Format %%ld of 10l gives %s", buffer);

        sprintf(buffer, "%lld", 10ll);
        TEST(strcmp(buffer, "10") == 0, "Format %%lld of 10ll gives %s", buffer);

        sprintf(buffer, "%lld", -10ll);
        TEST(strcmp(buffer, "-10") == 0, "Format %%lld of -10ll gives %s", buffer);

        sprintf(buffer, "%hd", (short)10);
        TEST(strcmp(buffer, "10") == 0, "Format %%hd of 10 gives %s", buffer);

        sprintf(buffer, "%u", 10);
        TEST(strcmp(buffer, "10") == 0, "Format %%u of 10 gives %s", buffer);

        sprintf(buffer, "%lu", 10l);
        TEST(strcmp(buffer, "10") == 0, "Format %%lu of 10l gives %s", buffer);

        sprintf(buffer, "%llu", 10ll);
        TEST(strcmp(buffer, "10") == 0, "Format %%llu of 10ll gives %s", buffer);

        sprintf(buffer, "%e", 10.0);
        TEST(strcmp(buffer, "1.000000e+01") == 0, "Format %%e of 10.0 gives %s", buffer);

        sprintf(buffer, "%E", 10.0);
        TEST(strcmp(buffer, "1.000000E+01") == 0, "Format %%E of 10.0 gives %s", buffer);

        sprintf(buffer, "%e", 0.010);
        TEST(strcmp(buffer, "1.000000e-02") == 0, "Format %%e of 0.0100 gives %s", buffer);

        sprintf(buffer, "%g", 1e50);
        TEST(strcmp(buffer, "1.000000e+50") == 0, "Format %%g of 1e50 gives %s", buffer);

        sprintf(buffer, "%e", 1e50);
        TEST(strcmp(buffer, "1.000000e+50") == 0, "Format %%e of 1e50 gives %s", buffer);

        sprintf(buffer, "%e", 1e150);
        TEST(strcmp(buffer, "1.000000e+150") == 0, "Format %%e of 1e150 gives %s", buffer);

        sprintf(buffer, "%f", 10.0);
        TEST(strcmp(buffer, "10.000000") == 0, "Format %%f of 10.0 gives %s", buffer);

        sprintf(buffer, "%g", 10.0);
        TEST(strcmp(buffer, "10") == 0, "Format %%g of 10.0 gives %s", buffer);

        sprintf(buffer, "%g", -10.0);
        TEST(strcmp(buffer, "-10") == 0, "Format %%g of -10.0 gives %s", buffer);

        sprintf(buffer, "%g", 1000000000.0);
        TEST(strcmp(buffer, "1e+09") == 0, "Format %%e of 1000000000.0 gives %s", buffer);

        sprintf(buffer, "%G", 1000000000.0);
        TEST(strcmp(buffer, "1E+09") == 0, "Format %%E of 1000000000.0 gives %s", buffer);

        sprintf(buffer, "%g", 0.1);
        TEST(strcmp(buffer, "0.1") == 0, "Format %%g of 0.1 gives %s", buffer);

        sprintf(buffer, "%g", 9.99999999);
        TEST(strcmp(buffer, "10") == 0, "Format %%g of 9.99999999 gives %s", buffer);

        sprintf(buffer, "%10.9g", 9.99999999);
        TEST(strcmp(buffer, "9.99999999") == 0, "Format %%10.9g of 9.99999999 gives %s", buffer);

        sprintf(buffer, "%*.9g", 10, 9.99999999);
        TEST(strcmp(buffer, "9.99999999") == 0, "Format %%*.9g of 9.99999999 gives %s", buffer);

        sprintf(buffer, "%10.*g", 9, 9.99999999);
        TEST(strcmp(buffer, "9.99999999") == 0, "Format %%10.*g of 9.99999999 gives %s", buffer);

        sprintf(buffer, "%*.g", 10, 9.99999999);
        TEST(strcmp(buffer, "     1e+01") == 0, "Format %%*.g of 9.99999999 gives %s", buffer);

        sprintf(buffer, "%g", 0.01);
        TEST(strcmp(buffer, "0.01") == 0, "Format %%g of 0.01 gives %s", buffer);

        sprintf(buffer, "%x", 10);
        TEST(strcmp(buffer, "a") == 0, "Format %%x of 10 gives %s", buffer);

        sprintf(buffer, "%lx", 10l);
        TEST(strcmp(buffer, "a") == 0, "Format %%lx of 10 gives %s", buffer);

        sprintf(buffer, "%llx", 10ll);
        TEST(strcmp(buffer, "a") == 0, "Format %%llx of 10 gives %s", buffer);

        sprintf(buffer, "%#x", 10);
        TEST(strcmp(buffer, "0xa") == 0, "Format %%#x of 10 gives %s", buffer);

        sprintf(buffer, "%#X", 10);
        TEST(strcmp(buffer, "0XA") == 0, "Format %%#X of 10 gives %s", buffer);

        sprintf(buffer, "%X", 10);
        TEST(strcmp(buffer, "A") == 0, "Format %%X of 10 gives %s", buffer);

        sprintf(buffer, "%o", 10);
        TEST(strcmp(buffer, "12") == 0, "Format %%o of 10 gives %s", buffer);

        sprintf(buffer, "%#o", 10);
        TEST(strcmp(buffer, "012") == 0, "Format %%#o of 10 gives %s", buffer);

        sprintf(buffer, "%#o", 0);
        TEST(strcmp(buffer, "0") == 0, "Format %%#o of 0 gives %s", buffer);

        sprintf(buffer, "%lo", 10l);
        TEST(strcmp(buffer, "12") == 0, "Format %%lo of 10 gives %s", buffer);

        sprintf(buffer, "%llo", 10ll);
        TEST(strcmp(buffer, "12") == 0, "Format %%llo of 10 gives %s", buffer);

        {
        int len;
        sprintf(buffer, "%d%n", 10, &len);
        TEST(strcmp(buffer, "10") == 0, "Format %%d%%n of 10 gives %s", buffer);
        TEST(len == 2, "Check format length");
        }

        {
        long len;
        sprintf(buffer, "%d%ln", 10, &len);
        TEST(strcmp(buffer, "10") == 0, "Format %%d%%ln of 10 gives %s", buffer);
        TEST(len == 2, "Check format length gives %ld", len);
        }

        {
        long long len;
        sprintf(buffer, "%d%lln", 10, &len);
        TEST(strcmp(buffer, "10") == 0, "Format %%d%%lln of 10 gives %s", buffer);
        TEST(len == 2, "Check format length");
        }

        {
        short len;
        sprintf(buffer, "%d%hn", 10, &len);
        TEST(strcmp(buffer, "10") == 0, "Format %%d%%hn of 10 gives %s", buffer);
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
    }


    TEST_DONE();
}
