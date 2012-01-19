/* ===-- mulsi3.c - Implement __mulsi3 -------------------------------------===
 *
 *                     The LLVM Compiler Infrastructure
 *
 * This file is dual licensed under the MIT and the University of Illinois Open
 * Source Licenses. See LICENSE.TXT for details.
 *
 * ===----------------------------------------------------------------------===
 *
 * This file implements __mulsi3 for the compiler_rt library.
 *
 * ===----------------------------------------------------------------------===
 */ 

#include "int_lib.h"

/* Returns: a * b */

si_int
__mulsi3(su_int a, su_int b)
{
    si_int r = 0;
    while (a) {
        if (a & 1) {
            r += b;
        }
        b <<= 1;
        a >>= 1;
    }

    return r;
}
