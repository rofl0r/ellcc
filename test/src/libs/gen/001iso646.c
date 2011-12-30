#include <ecc_test.h>
#include <iso646.h>
#include <string.h>

#define xstr(x) #x
#define str(x) xstr(x)

#define DOTEST(name, op) \
    TEST(strcmp(str(name), #op) == 0, #name " == " str(#op))
TEST_GROUP(ISO646)
    // C99 7.9
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

