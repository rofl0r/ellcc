// XFAIL: *
// RUN: %ppcecc -o %t %s && %ppcrun %t
#include "../ecc_test.h"
#include <stdio.h>
#include <string.h>

// Check formatting.
#define chk(fmt, out, ...)                                                      \
    do {                                                                        \
        char buffer[10240];                                                     \
        sprintf(buffer, fmt, __VA_ARGS__);                                      \
        TEST(strcmp(buffer, out) == 0, "Format \"%s\" with ("                   \
                                       #__VA_ARGS__                             \
                                       ") gives \"%s\"", fmt, buffer);          \
    } while (0)

TEST_GROUP(Format)
    chk("%g", "1", 1.0);
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
END_GROUP
