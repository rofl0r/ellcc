#include <ecc_test.h>
#include <ctype.h>

#define IS !!
#define ISNOT !
#define LC "abcdefghijklmnopqrstuvwxyz"
#define UC "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DIGIT "0123456789"
#define PUNCT "~!@#$%^&*()_+`-={}|[]\\:\";'<>?,./"
#define XDIGIT DIGIT "abcdefABCDEF"
#define BLANK " \t"
#define CNTRL "\t\n\r\f\v"
#define ALPHA LC UC
#define WS BLANK "\n\r\f\v"
#define ALNUM ALPHA DIGIT
#define GRAPH ALNUM PUNCT
#define PRINT GRAPH " "
#define ALL WS GRAPH
#define ALLLC WS PUNCT DIGIT LC
#define ALLUC WS PUNCT DIGIT UC

#define CTESTGROUP(group, ok, test) \
    do { \
        const char *p = group; \
        int result = 1; \
        while (*p) { \
            if (!test(*p++)) { \
                result = 0; \
                break; \
            } \
        } \
        TEST(ok result, "Group " #group " " #ok " " #test); \
    } while(0)

#define CTESTMAP(from, to, test) \
    do { \
        const char *p = from; \
        const char *q = to; \
        int result = 1; \
        while (*p) { \
            if (test(*p++) != *q++) { \
                result = 0; \
                break; \
            } \
        } \
        TEST(result, "Map from " #from " to " #to " " #test); \
    } while(0)

TEST_GROUP(Ctype)
    TEST_TRACE(C99 7.4.1.1)
    CTESTGROUP(ALL, ISNOT, isalnum);
    CTESTGROUP(ALNUM, IS, isalnum);
    CTESTGROUP(ALPHA, IS, isalnum);
    CTESTGROUP(BLANK, ISNOT, isalnum);
    CTESTGROUP(CNTRL, ISNOT, isalnum);
    CTESTGROUP(DIGIT, IS, isalnum);
    CTESTGROUP(GRAPH, ISNOT, isalnum);
    CTESTGROUP(LC, IS, isalnum);
    CTESTGROUP(UC, IS, isalnum);
    CTESTGROUP(WS, ISNOT, isalnum);
    CTESTGROUP(PUNCT, ISNOT, isalnum);
    CTESTGROUP(PRINT, ISNOT, isalnum);
    CTESTGROUP(XDIGIT, IS, isalnum);

    TEST_TRACE(C99 7.4.1.2)
    CTESTGROUP(ALL, ISNOT, isalpha);
    CTESTGROUP(ALNUM, ISNOT, isalpha);
    CTESTGROUP(ALPHA, IS, isalpha);
    CTESTGROUP(BLANK, ISNOT, isalnum);
    CTESTGROUP(CNTRL, ISNOT, isalnum);
    CTESTGROUP(DIGIT, ISNOT, isalpha);
    CTESTGROUP(GRAPH, ISNOT, isalpha);
    CTESTGROUP(LC, IS, isalpha);
    CTESTGROUP(UC, IS, isalpha);
    CTESTGROUP(WS, ISNOT, isalpha);
    CTESTGROUP(PUNCT, ISNOT, isalpha);
    CTESTGROUP(PRINT, ISNOT, isalpha);
    CTESTGROUP(XDIGIT, ISNOT, isalpha);

    TEST_TRACE(C99 7.4.1.3)
    CTESTGROUP(ALL, ISNOT, isblank);
    CTESTGROUP(ALNUM, ISNOT, isblank);
    CTESTGROUP(ALPHA, ISNOT, isblank);
    CTESTGROUP(BLANK, IS, isblank);
    CTESTGROUP(CNTRL, ISNOT, isblank);
    CTESTGROUP(DIGIT, ISNOT, isblank);
    CTESTGROUP(GRAPH, ISNOT, isblank);
    CTESTGROUP(LC, ISNOT, isblank);
    CTESTGROUP(UC, ISNOT, isblank);
    CTESTGROUP(WS, ISNOT, isblank);
    CTESTGROUP(PUNCT, ISNOT, isblank);
    CTESTGROUP(PRINT, ISNOT, isblank);
    CTESTGROUP(XDIGIT, ISNOT, isblank);

    TEST_TRACE(C99 7.4.1.4)
    CTESTGROUP(ALL, ISNOT, iscntrl);
    CTESTGROUP(ALNUM, ISNOT, iscntrl);
    CTESTGROUP(ALPHA, ISNOT, iscntrl);
    CTESTGROUP(BLANK, ISNOT, iscntrl);
    CTESTGROUP(CNTRL, IS, iscntrl);
    CTESTGROUP(DIGIT, ISNOT, iscntrl);
    CTESTGROUP(GRAPH, ISNOT, iscntrl);
    CTESTGROUP(LC, ISNOT, iscntrl);
    CTESTGROUP(UC, ISNOT, iscntrl);
    CTESTGROUP(WS, ISNOT, iscntrl);
    CTESTGROUP(PUNCT, ISNOT, iscntrl);
    CTESTGROUP(PRINT, ISNOT, iscntrl);
    CTESTGROUP(XDIGIT, ISNOT, iscntrl);

    TEST_TRACE(C99 7.4.1.5)
    CTESTGROUP(ALL, ISNOT, isdigit);
    CTESTGROUP(ALNUM, ISNOT, isdigit);
    CTESTGROUP(ALPHA, ISNOT, isdigit);
    CTESTGROUP(BLANK, ISNOT, isdigit);
    CTESTGROUP(CNTRL, ISNOT, isdigit);
    CTESTGROUP(DIGIT, IS, isdigit);
    CTESTGROUP(GRAPH, ISNOT, isdigit);
    CTESTGROUP(LC, ISNOT, isdigit);
    CTESTGROUP(UC, ISNOT, isdigit);
    CTESTGROUP(WS, ISNOT, isdigit);
    CTESTGROUP(PUNCT, ISNOT, isdigit);
    CTESTGROUP(PRINT, ISNOT, isdigit);
    CTESTGROUP(XDIGIT, ISNOT, isdigit);

    TEST_TRACE(C99 7.4.1.6)
    CTESTGROUP(ALL, ISNOT, isgraph);
    CTESTGROUP(ALNUM, IS, isgraph);
    CTESTGROUP(ALPHA, IS, isgraph);
    CTESTGROUP(BLANK, ISNOT, isgraph);
    CTESTGROUP(CNTRL, ISNOT, isgraph);
    CTESTGROUP(DIGIT, IS, isgraph);
    CTESTGROUP(GRAPH, IS, isgraph);
    CTESTGROUP(LC, IS, isgraph);
    CTESTGROUP(UC, IS, isgraph);
    CTESTGROUP(WS, ISNOT, isgraph);
    CTESTGROUP(PUNCT, IS, isgraph);
    CTESTGROUP(PRINT, ISNOT, isgraph);
    CTESTGROUP(XDIGIT, IS, isgraph);

    TEST_TRACE(C99 7.4.1.7)
    CTESTGROUP(ALL, ISNOT, islower);
    CTESTGROUP(ALNUM, ISNOT, islower);
    CTESTGROUP(ALPHA, ISNOT, islower);
    CTESTGROUP(BLANK, ISNOT, islower);
    CTESTGROUP(CNTRL, ISNOT, islower);
    CTESTGROUP(DIGIT, ISNOT, islower);
    CTESTGROUP(GRAPH, ISNOT, islower);
    CTESTGROUP(LC, IS, islower);
    CTESTGROUP(UC, ISNOT, islower);
    CTESTGROUP(WS, ISNOT, islower);
    CTESTGROUP(PUNCT, ISNOT, islower);
    CTESTGROUP(PRINT, ISNOT, islower);
    CTESTGROUP(XDIGIT, ISNOT, islower);

    TEST_TRACE(C99 7.4.1.8)
    CTESTGROUP(ALL, ISNOT, isprint);
    CTESTGROUP(ALNUM, IS, isprint);
    CTESTGROUP(ALPHA, IS, isprint);
    CTESTGROUP(BLANK, ISNOT, isprint);
    CTESTGROUP(CNTRL, ISNOT, isprint);
    CTESTGROUP(DIGIT, IS, isprint);
    CTESTGROUP(GRAPH, IS, isprint);
    CTESTGROUP(LC, IS, isprint);
    CTESTGROUP(UC, IS, isprint);
    CTESTGROUP(WS, ISNOT, isprint);
    CTESTGROUP(PUNCT, IS, isprint);
    CTESTGROUP(PRINT, IS, isprint);
    CTESTGROUP(XDIGIT, IS, isprint);
    
    TEST_TRACE(C99 7.4.1.9)
    CTESTGROUP(ALL, ISNOT, ispunct);
    CTESTGROUP(ALNUM, ISNOT, ispunct);
    CTESTGROUP(ALPHA, ISNOT, ispunct);
    CTESTGROUP(BLANK, ISNOT, ispunct);
    CTESTGROUP(CNTRL, ISNOT, ispunct);
    CTESTGROUP(DIGIT, ISNOT, ispunct);
    CTESTGROUP(GRAPH, ISNOT, ispunct);
    CTESTGROUP(LC, ISNOT, ispunct);
    CTESTGROUP(UC, ISNOT, ispunct);
    CTESTGROUP(WS, ISNOT, ispunct);
    CTESTGROUP(PUNCT, IS, ispunct);
    CTESTGROUP(PRINT, ISNOT, ispunct);
    CTESTGROUP(XDIGIT, ISNOT, ispunct);

    TEST_TRACE(C99 7.4.1.10)
    CTESTGROUP(ALL, ISNOT, isspace);
    CTESTGROUP(ALNUM, ISNOT, isspace);
    CTESTGROUP(ALPHA, ISNOT, isspace);
    CTESTGROUP(BLANK, IS, isspace);
    CTESTGROUP(CNTRL, IS, isspace);
    CTESTGROUP(DIGIT, ISNOT, isspace);
    CTESTGROUP(GRAPH, ISNOT, isspace);
    CTESTGROUP(LC, ISNOT, isspace);
    CTESTGROUP(UC, ISNOT, isspace);
    CTESTGROUP(WS, IS, isspace);
    CTESTGROUP(PUNCT, ISNOT, isspace);
    CTESTGROUP(PRINT, ISNOT, isspace);
    CTESTGROUP(XDIGIT, ISNOT, isspace);

    TEST_TRACE(C99 7.4.1.11)
    CTESTGROUP(ALL, ISNOT, isupper);
    CTESTGROUP(ALNUM, ISNOT, isupper);
    CTESTGROUP(ALPHA, ISNOT, isupper);
    CTESTGROUP(BLANK, ISNOT, isupper);
    CTESTGROUP(CNTRL, ISNOT, isupper);
    CTESTGROUP(DIGIT, ISNOT, isupper);
    CTESTGROUP(GRAPH, ISNOT, isupper);
    CTESTGROUP(LC, ISNOT, isupper);
    CTESTGROUP(UC, IS, isupper);
    CTESTGROUP(WS, ISNOT, isupper);
    CTESTGROUP(PUNCT, ISNOT, isupper);
    CTESTGROUP(PRINT, ISNOT, isupper);
    CTESTGROUP(XDIGIT, ISNOT, isupper);

    TEST_TRACE(C99 7.4.1.12)
    CTESTGROUP(ALL, ISNOT, isxdigit);
    CTESTGROUP(ALNUM, ISNOT, isxdigit);
    CTESTGROUP(ALPHA, ISNOT, isxdigit);
    CTESTGROUP(BLANK, ISNOT, isxdigit);
    CTESTGROUP(CNTRL, ISNOT, isxdigit);
    CTESTGROUP(DIGIT, IS, isxdigit);
    CTESTGROUP(GRAPH, ISNOT, isxdigit);
    CTESTGROUP(LC, ISNOT, isxdigit);
    CTESTGROUP(UC, ISNOT, isxdigit);
    CTESTGROUP(WS, ISNOT, isxdigit);
    CTESTGROUP(PUNCT, ISNOT, isxdigit);
    CTESTGROUP(PRINT, ISNOT, isxdigit);
    CTESTGROUP(XDIGIT, IS, isxdigit);

    TEST_TRACE(C99 7.4.1.1)
    CTESTMAP(UC, LC, tolower);
    CTESTMAP(ALLUC, ALLLC, tolower);

    TEST_TRACE(C99 7.4.1.2)
    CTESTMAP(LC, UC, toupper);
    CTESTMAP(ALLLC, ALLUC, toupper);
END_GROUP

