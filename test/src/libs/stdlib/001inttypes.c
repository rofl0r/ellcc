#include <ecc_test.h>
#include <inttypes.h>

TEST_GROUP(Inttypes)
    char *p;
    TEST_TRACE(C99 7.8)
    TEST_TRACE(C99 7.8/2)
    imaxdiv_t maxdiv;
    TEST_TRACE(C99 7.8.1/2)
    p = PRIdMAX;
    p = PRIiMAX;
    p = PRIoMAX;
    p = PRIuMAX;
    p = PRIxMAX;
    p = PRIXMAX;
    p = PRIdPTR;
    p = PRIiPTR;
    p = PRIoPTR;
    p = PRIuPTR;
    p = PRIxPTR;
    p = PRIXPTR;
    p = SCNdPTR;
    p = SCNiPTR;
    p = SCNoPTR;
    p = SCNuPTR;
    p = SCNxPTR;
#ifdef __INT64_TYPE__
    p = PRId64;
    p = PRIdLEAST64;
    p = PRIdFAST64;
    p = PRIi64;
    p = PRIiLEAST64;
    p = PRIiFAST64;
    p = PRIo64;
    p = PRIoLEAST64;
    p = PRIoFAST64;
    p = PRIu64;
    p = PRIuLEAST64;
    p = PRIuFAST64;
    p = PRIx64;
    p = PRIxLEAST64;
    p = PRIxFAST64;
    p = PRIX64;
    p = PRIXLEAST64;
    p = PRIXFAST64;
    p = SCNd64;
    p = SCNdLEAST64;
    p = SCNdFAST64;
    p = SCNi64;
    p = SCNiLEAST64;
    p = SCNiFAST64;
    p = SCNo64;
    p = SCNoLEAST64;
    p = SCNoFAST64;
    p = SCNu64;
    p = SCNuLEAST64;
    p = SCNuFAST64;
    p = SCNx64;
    p = SCNxLEAST64;
    p = SCNxFAST64;
#endif
#ifdef __INT56_TYPE__
    p = PRId56;
    p = PRIdLEAST56;
    p = PRIdFAST56;
    p = PRIi56;
    p = PRIiLEAST56;
    p = PRIiFAST56;
    p = PRIo56;
    p = PRIoLEAST56;
    p = PRIoFAST56;
    p = PRIu56;
    p = PRIuLEAST56;
    p = PRIuFAST56;
    p = PRIx56;
    p = PRIxLEAST56;
    p = PRIxFAST56;
    p = PRIX56;
    p = PRIXLEAST56;
    p = PRIXFAST56;
    p = SCNd56;
    p = SCNdLEAST56;
    p = SCNdFAST56;
    p = SCNi56;
    p = SCNiLEAST56;
    p = SCNiFAST56;
    p = SCNo56;
    p = SCNoLEAST56;
    p = SCNoFAST56;
    p = SCNu56;
    p = SCNuLEAST56;
    p = SCNuFAST56;
    p = SCNx56;
    p = SCNxLEAST56;
    p = SCNxFAST56;
#endif
#ifdef __INT48_TYPE__
    p = PRId48;
    p = PRIdLEAST48;
    p = PRIdFAST48;
    p = PRIi48;
    p = PRIiLEAST48;
    p = PRIiFAST48;
    p = PRIo48;
    p = PRIoLEAST48;
    p = PRIoFAST48;
    p = PRIu48;
    p = PRIuLEAST48;
    p = PRIuFAST48;
    p = PRIx48;
    p = PRIxLEAST48;
    p = PRIxFAST48;
    p = PRIX48;
    p = PRIXLEAST48;
    p = PRIXFAST48;
    p = SCNd48;
    p = SCNdLEAST48;
    p = SCNdFAST48;
    p = SCNi48;
    p = SCNiLEAST48;
    p = SCNiFAST48;
    p = SCNo48;
    p = SCNoLEAST48;
    p = SCNoFAST48;
    p = SCNu48;
    p = SCNuLEAST48;
    p = SCNuFAST48;
    p = SCNx48;
    p = SCNxLEAST48;
    p = SCNxFAST48;
#endif
#ifdef __INT40_TYPE__
    p = PRId40;
    p = PRIdLEAST40;
    p = PRIdFAST40;
    p = PRIi40;
    p = PRIiLEAST40;
    p = PRIiFAST40;
    p = PRIo40;
    p = PRIoLEAST40;
    p = PRIoFAST40;
    p = PRIu40;
    p = PRIuLEAST40;
    p = PRIuFAST40;
    p = PRIx40;
    p = PRIxLEAST40;
    p = PRIxFAST40;
    p = PRIX40;
    p = PRIXLEAST40;
    p = PRIXFAST40;
    p = SCNd40;
    p = SCNdLEAST40;
    p = SCNdFAST40;
    p = SCNi40;
    p = SCNiLEAST40;
    p = SCNiFAST40;
    p = SCNo40;
    p = SCNoLEAST40;
    p = SCNoFAST40;
    p = SCNu40;
    p = SCNuLEAST40;
    p = SCNuFAST40;
    p = SCNx40;
    p = SCNxLEAST40;
    p = SCNxFAST40;
#endif
#ifdef __INT32_TYPE__
    p = PRId32;
    p = PRIdLEAST32;
    p = PRIdFAST32;
    p = PRIi32;
    p = PRIiLEAST32;
    p = PRIiFAST32;
    p = PRIo32;
    p = PRIoLEAST32;
    p = PRIoFAST32;
    p = PRIu32;
    p = PRIuLEAST32;
    p = PRIuFAST32;
    p = PRIx32;
    p = PRIxLEAST32;
    p = PRIxFAST32;
    p = PRIX32;
    p = PRIXLEAST32;
    p = PRIXFAST32;
    p = SCNd32;
    p = SCNdLEAST32;
    p = SCNdFAST32;
    p = SCNi32;
    p = SCNiLEAST32;
    p = SCNiFAST32;
    p = SCNo32;
    p = SCNoLEAST32;
    p = SCNoFAST32;
    p = SCNu32;
    p = SCNuLEAST32;
    p = SCNuFAST32;
    p = SCNx32;
    p = SCNxLEAST32;
    p = SCNxFAST32;
#endif
#ifdef __INT24_TYPE__
    p = PRId24;
    p = PRIdLEAST24;
    p = PRIdFAST24;
    p = PRIi24;
    p = PRIiLEAST24;
    p = PRIiFAST24;
    p = PRIo24;
    p = PRIoLEAST24;
    p = PRIoFAST24;
    p = PRIu24;
    p = PRIuLEAST24;
    p = PRIuFAST24;
    p = PRIx24;
    p = PRIxLEAST24;
    p = PRIxFAST24;
    p = PRIX24;
    p = PRIXLEAST24;
    p = PRIXFAST24;
    p = SCNd24;
    p = SCNdLEAST24;
    p = SCNdFAST24;
    p = SCNi24;
    p = SCNiLEAST24;
    p = SCNiFAST24;
    p = SCNo24;
    p = SCNoLEAST24;
    p = SCNoFAST24;
    p = SCNu24;
    p = SCNuLEAST24;
    p = SCNuFAST24;
    p = SCNx24;
    p = SCNxLEAST24;
    p = SCNxFAST24;
#endif
#ifdef __INT16_TYPE__
    p = PRId16;
    p = PRIdLEAST16;
    p = PRIdFAST16;
    p = PRIi16;
    p = PRIiLEAST16;
    p = PRIiFAST16;
    p = PRIo16;
    p = PRIoLEAST16;
    p = PRIoFAST16;
    p = PRIu16;
    p = PRIuLEAST16;
    p = PRIuFAST16;
    p = PRIx16;
    p = PRIxLEAST16;
    p = PRIxFAST16;
    p = PRIX16;
    p = PRIXLEAST16;
    p = PRIXFAST16;
    p = SCNd16;
    p = SCNdLEAST16;
    p = SCNdFAST16;
    p = SCNi16;
    p = SCNiLEAST16;
    p = SCNiFAST16;
    p = SCNo16;
    p = SCNoLEAST16;
    p = SCNoFAST16;
    p = SCNu16;
    p = SCNuLEAST16;
    p = SCNuFAST16;
    p = SCNx16;
    p = SCNxLEAST16;
    p = SCNxFAST16;
#endif
#ifdef __INT8_TYPE__
    p = PRId8;
    p = PRIdLEAST8;
    p = PRIdFAST8;
    p = PRIi8;
    p = PRIiLEAST8;
    p = PRIiFAST8;
    p = PRIo8;
    p = PRIoLEAST8;
    p = PRIoFAST8;
    p = PRIu8;
    p = PRIuLEAST8;
    p = PRIuFAST8;
    p = PRIx8;
    p = PRIxLEAST8;
    p = PRIxFAST8;
    p = PRIX8;
    p = PRIXLEAST8;
    p = PRIXFAST8;
    p = SCNd8;
    p = SCNdLEAST8;
    p = SCNdFAST8;
    p = SCNi8;
    p = SCNiLEAST8;
    p = SCNiFAST8;
    p = SCNo8;
    p = SCNoLEAST8;
    p = SCNoFAST8;
    p = SCNu8;
    p = SCNuLEAST8;
    p = SCNuFAST8;
    p = SCNx8;
    p = SCNxLEAST8;
    p = SCNxFAST8;
#endif
    TEST_TRACE(C99 7.8.2.1)
    TEST(imaxabs((intmax_t)-10) == 10, "imaxabs of -10 is 10");
    TEST(imaxabs((intmax_t)10) == 10, "imaxabs of 10 is 10");
    TEST_TRACE(C99 7.8.2.2)
    maxdiv = imaxdiv((intmax_t)123, (intmax_t)10);
    TEST(maxdiv.quot == 12, "123 / 10 = 12");
    TEST(maxdiv.rem == 3, "123 %% 10 = 3");
    TEST_TRACE(C99 7.8.2.3)
    intmax_t m;
    m = strtoimax("100", 0, 0);
    TEST(m == 100, "strtoimax(\"100\", 0, 0) == 100");
    uintmax_t um;
    um = strtoumax("100", 0, 0);
    TEST(um == 100, "strtoumax(\"100\", 0, 0) == 100");
#define BIGNUM "100000000000000000000000000000000000000"
    m = strtoimax(BIGNUM, 0, 0);
    TEST(m == INTMAX_MAX, "strtoimax(BIGNUM, 0, 0) == INTMAX_MAX");
    TEST_EXCLUDE(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=15") {
        m = strtoimax("-" BIGNUM, 0, 0);
        TEST(m == INTMAX_MIN, "strtoimax(L\"-\" BIGNUM, 0, 0) == INTMAX_MIN");
    }
    um = strtoumax(BIGNUM, 0, 0);
    TEST(um == UINTMAX_MAX, "strtoumax(BIGNUM, 0, 0) == UINTMAX_MAX");
    TEST_TRACE(C99 7.8.2.4)
    m = wcstoimax(L"100", 0, 0);
    TEST(m == 100, "wcstoimax(L\"100\", 0, 0) == 100");
    um = wcstoumax(L"100", 0, 0);
    TEST(um == 100, "wcstoumax(L\"100\", 0, 0) == 100");
#define WBIGNUM L"100000000000000000000000000000000000000"
#define WMBIGNUM L"-100000000000000000000000000000000000000"
    m = wcstoimax(WBIGNUM, 0, 0);
    TEST(m == INTMAX_MAX, "wcstoimax(WBIGNUM, 0, 0) == INTMAX_MAX");
    TEST_EXCLUDE(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=15") {
    m = wcstoimax(WMBIGNUM, 0, 0);
    TEST(m == INTMAX_MIN, "wcstoimax(WMBIGNUM, 0, 0) == INTMAX_MIN");
    }
    um = wcstoumax(WBIGNUM, 0, 0);
    TEST(um == UINTMAX_MAX, "wcstoumax(WBIGNUM, 0, 0) == UINTMAX_MAX");
END_GROUP

