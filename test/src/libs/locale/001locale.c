#include <ecc_test.h>
#include <locale.h>
#include <limits.h>

TEST_GROUP(Locale)
    TEST_TRACE(C99 7.11)
    TEST_TRACE(C99 7.11/2)
    static struct lconv check = {
        .decimal_point = ".",
        .thousands_sep = "",
        .grouping = "",
        .mon_decimal_point = "",
        .mon_thousands_sep = "",
        .mon_grouping = "",
        .positive_sign = "",
        .negative_sign = "",
        .currency_symbol = "",
        .frac_digits = CHAR_MAX,
        .p_cs_precedes = CHAR_MAX,
        .n_cs_precedes = CHAR_MAX,
        .p_sep_by_space = CHAR_MAX,
        .n_sep_by_space = CHAR_MAX,
        .p_sign_posn = CHAR_MAX,
        .n_sign_posn = CHAR_MAX,
        .int_curr_symbol = "",
        .int_frac_digits = CHAR_MAX,
        .int_p_cs_precedes = CHAR_MAX,
        .int_n_cs_precedes = CHAR_MAX,
        .int_p_sep_by_space = CHAR_MAX,
        .int_n_sep_by_space = CHAR_MAX,
        .int_p_sign_posn = CHAR_MAX,
        .int_n_sign_posn = CHAR_MAX,
    };
 
    TEST_TRACE(C99 7.11/3)
    TEST(LC_ALL != LC_COLLATE && LC_ALL != LC_CTYPE
      && LC_ALL != LC_MONETARY && LC_ALL != LC_NUMERIC
      && LC_ALL != LC_TIME, "LC_ALL has a distinct value");
    TEST(LC_COLLATE != LC_CTYPE
      && LC_COLLATE != LC_MONETARY && LC_COLLATE != LC_NUMERIC
      && LC_COLLATE != LC_TIME, "LC_COLLATE has a distinct value");
    TEST(LC_CTYPE != LC_MONETARY && LC_CTYPE != LC_NUMERIC
      && LC_CTYPE != LC_TIME, "LC_CTYPE has a distinct value");
    TEST(LC_MONETARY != LC_NUMERIC
      && LC_MONETARY != LC_TIME, "LC_MONETARY has a distinct value");
    TEST(LC_NUMERIC != LC_TIME, "LC_NUMERIC has a distinct value");

    TEST_TRACE(C99 7.11.1)
    char *current = setlocale(LC_ALL, "C");
    // RICH: TEST(current != 0, "setlocale(LC_ALL, \"C\") returns a string");
    TEST_TRACE(C99 7.11.2)
    struct lconv *lconv;
    lconv = localeconv();
    TEST(lconv != 0, "localconv() returns a pointer");
END_GROUP

