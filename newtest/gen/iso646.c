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
#include <iso646.h>
#include <string.h>

#define xstr(x) #x
#define str(x) xstr(x)

#define DOTEST(name, op) \
    TEST(strcmp(str(name), #op) == 0, #name " == " str(#op))
TEST_GROUP(ISO646)
    TEST_TRACE(C99 7.9)
    DOTEST(and, &&);
    DOTEST(and_eq, &=);
    DOTEST(bitand, &);
    DOTEST(bitor, |);
    DOTEST(compl, ~);
    DOTEST(not, !);
    DOTEST(not_eq, !=);
    DOTEST(or, ||);
    DOTEST(or_eq, |=);
    DOTEST(xor, ^);
    DOTEST(xor_eq, ^=);
END_GROUP

