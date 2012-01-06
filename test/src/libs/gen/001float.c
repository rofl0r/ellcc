#include <ecc_test.h>
#include <float.h>

TEST_GROUP(Float)
    int i;
    TEST_TRACE(C99 7.7)
    TEST_TRACE(C99 5.2.4.2.2/6)
    TEST(FLT_ROUNDS >= -1 && FLT_ROUNDS <= 3, "FLT_ROUNDS is OK");
    TEST_TRACE(C99 5.2.4.2.2/7)
    TEST(FLT_EVAL_METHOD >= -1 && FLT_EVAL_METHOD <= 2, "FLT_EVAL_METHOD is OK");
    TEST_TRACE(C99 5.2.4.2.2/8)
    TEST(FLT_RADIX >= 2, "The radix is >= 2");
    i = FLT_MANT_DIG;
    i = DBL_MANT_DIG;
    i = LDBL_MANT_DIG;
    TEST(DECIMAL_DIG >= 10, "The maximum number of decimal digits is >= 10");
    TEST(FLT_DIG >= 6, "The number of float decimal digits is >= 6");
    TEST(DBL_DIG >= 10, "The number of double decimal digits is >= 10");
    TEST(LDBL_DIG >= 10, "The number of long double decimal digits is >= 10");
    i = FLT_MIN_EXP;
    i = DBL_MIN_EXP;
    i = LDBL_MIN_EXP;
    TEST(FLT_MIN_10_EXP <= -37, "The float minimum base 10 exponent <= -37");
    TEST(DBL_MIN_10_EXP <= -37, "The double minimum base 10 exponent <= -37");
    TEST(LDBL_MIN_10_EXP <= -37, "The long double minimum base 10 exponent <= -37");
    i = FLT_MAX_EXP;
    i = DBL_MAX_EXP;
    i = LDBL_MAX_EXP;
    TEST(FLT_MAX_10_EXP >= 37, "The float maximum base 10 exponent >= 37");
    TEST(DBL_MAX_10_EXP >= 37, "The double maximum base 10 exponent >= 37");
    TEST(LDBL_MAX_10_EXP >= 37, "The long double maximum base 10 exponent >= 37");
    TEST(FLT_MAX >= 1E37, "The float maximum value >= 1E37");
    TEST(DBL_MAX >= 1E37, "The double maximum value >= 1E37");
    TEST(LDBL_MAX >= 1E37, "The long double maximum value >= 1E37");
    TEST(FLT_EPSILON <= 1E-5, "The float epsilon <= 1E-5");
    TEST(DBL_EPSILON <= 1E-9, "The double epsilon <= 1E-9");
    TEST(LDBL_EPSILON <= 1E-9, "The long double epsilon <= 1E-9");
    TEST(FLT_MIN <= 1E-37, "The float maximum value <= 1E-37");
    TEST(DBL_MIN <= 1E-37, "The double maximum value <= 1E-37");
    TEST(LDBL_MIN <= 1E-37, "The long double maximum value <= 1E-37");
END_GROUP

