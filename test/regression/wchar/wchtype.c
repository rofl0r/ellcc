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
#include <wctype.h>

#define IS !!
#define ISNOT !
#define LC L"abcdefghijklmnopqrstuvwxyz"
#define UC L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DIGIT L"0123456789"
#define PUNCT L"~!@#$%^&*()_+`-={}|[]\\:\";'<>?,./"
#define XDIGIT DIGIT L"abcdefABCDEF"
#define BLANK L" \t"
#define CNTRL L"\t\n\r\f\v"
#define ALPHA LC UC
#define WS BLANK L"\n\r\f\v"
#define ALNUM ALPHA DIGIT
#define GRAPH ALNUM PUNCT
#define PRINT GRAPH L" "
#define ALL WS GRAPH
#define ALLLC WS PUNCT DIGIT LC
#define ALLUC WS PUNCT DIGIT UC

#define CTESTGROUP(group, ok, test) \
    do { \
        const wchar_t *p = group; \
        int result = 1; \
        while (*p) { \
            if (!test(*p++)) { \
                result = 0; \
                break; \
            } \
        } \
        TEST(ok result, "Group " #group " " #ok " " #test); \
    } while(0)

#define CTESTGROUP2(group, ok, test) \
    do { \
        const wchar_t *p = group; \
        int result = 1; \
        while (*p) { \
            if (!iswctype(*p++, wctype(#test))) { \
                result = 0; \
                break; \
            } \
        } \
        TEST(ok result, "Group " #group " " #ok " " #test); \
    } while(0)

#define CTESTMAP(from, to, test) \
    do { \
        const wchar_t *p = from; \
        const wchar_t *q = to; \
        int result = 1; \
        while (*p) { \
            if (test(*p++) != *q++) { \
                result = 0; \
                break; \
            } \
        } \
        TEST(result, "Map from " #from " to " #to " " #test); \
    } while(0)

#define CTESTMAP2(from, to, test) \
    do { \
        const wchar_t *p = from; \
        const wchar_t *q = to; \
        int result = 1; \
        while (*p) { \
            if (towctrans(*p++, wctrans(#test)) != *q++) { \
                result = 0; \
                break; \
            } \
        } \
        TEST(result, "Map from " #from " to " #to " " #test); \
    } while(0)

TEST_GROUP(Ctype)
    TEST_TRACE(C99 7.25.1/2)
    wint_t wint;
    wctrans_t trans;
    wctype_t type;
    TEST_TRACE(C99 7.25.1/3)
    wint = WEOF;
    TEST_TRACE(C99 7.25.2.1.1)
    CTESTGROUP(ALL, ISNOT, iswalnum);
    CTESTGROUP(ALNUM, IS, iswalnum);
    CTESTGROUP(ALPHA, IS, iswalnum);
    CTESTGROUP(BLANK, ISNOT, iswalnum);
    CTESTGROUP(CNTRL, ISNOT, iswalnum);
    CTESTGROUP(DIGIT, IS, iswalnum);
    CTESTGROUP(GRAPH, ISNOT, iswalnum);
    CTESTGROUP(LC, IS, iswalnum);
    CTESTGROUP(UC, IS, iswalnum);
    CTESTGROUP(WS, ISNOT, iswalnum);
    CTESTGROUP(PUNCT, ISNOT, iswalnum);
    CTESTGROUP(PRINT, ISNOT, iswalnum);
    CTESTGROUP(XDIGIT, IS, iswalnum);

    TEST_TRACE(C99 7.25.2.1.2)
    CTESTGROUP(ALL, ISNOT, iswalpha);
    CTESTGROUP(ALNUM, ISNOT, iswalpha);
    CTESTGROUP(ALPHA, IS, iswalpha);
    CTESTGROUP(BLANK, ISNOT, iswalnum);
    CTESTGROUP(CNTRL, ISNOT, iswalnum);
    CTESTGROUP(DIGIT, ISNOT, iswalpha);
    CTESTGROUP(GRAPH, ISNOT, iswalpha);
    CTESTGROUP(LC, IS, iswalpha);
    CTESTGROUP(UC, IS, iswalpha);
    CTESTGROUP(WS, ISNOT, iswalpha);
    CTESTGROUP(PUNCT, ISNOT, iswalpha);
    CTESTGROUP(PRINT, ISNOT, iswalpha);
    CTESTGROUP(XDIGIT, ISNOT, iswalpha);

    TEST_TRACE(C99 7.25.2.1.3)
    CTESTGROUP(ALL, ISNOT, iswblank);
    CTESTGROUP(ALNUM, ISNOT, iswblank);
    CTESTGROUP(ALPHA, ISNOT, iswblank);
    CTESTGROUP(BLANK, IS, iswblank);
    CTESTGROUP(CNTRL, ISNOT, iswblank);
    CTESTGROUP(DIGIT, ISNOT, iswblank);
    CTESTGROUP(GRAPH, ISNOT, iswblank);
    CTESTGROUP(LC, ISNOT, iswblank);
    CTESTGROUP(UC, ISNOT, iswblank);
    CTESTGROUP(WS, ISNOT, iswblank);
    CTESTGROUP(PUNCT, ISNOT, iswblank);
    CTESTGROUP(PRINT, ISNOT, iswblank);
    CTESTGROUP(XDIGIT, ISNOT, iswblank);

    TEST_TRACE(C99 7.25.2.1.4)
    CTESTGROUP(ALL, ISNOT, iswcntrl);
    CTESTGROUP(ALNUM, ISNOT, iswcntrl);
    CTESTGROUP(ALPHA, ISNOT, iswcntrl);
    CTESTGROUP(BLANK, ISNOT, iswcntrl);
    CTESTGROUP(CNTRL, IS, iswcntrl);
    CTESTGROUP(DIGIT, ISNOT, iswcntrl);
    CTESTGROUP(GRAPH, ISNOT, iswcntrl);
    CTESTGROUP(LC, ISNOT, iswcntrl);
    CTESTGROUP(UC, ISNOT, iswcntrl);
    CTESTGROUP(WS, ISNOT, iswcntrl);
    CTESTGROUP(PUNCT, ISNOT, iswcntrl);
    CTESTGROUP(PRINT, ISNOT, iswcntrl);
    CTESTGROUP(XDIGIT, ISNOT, iswcntrl);

    TEST_TRACE(C99 7.25.2.1.5)
    CTESTGROUP(ALL, ISNOT, iswdigit);
    CTESTGROUP(ALNUM, ISNOT, iswdigit);
    CTESTGROUP(ALPHA, ISNOT, iswdigit);
    CTESTGROUP(BLANK, ISNOT, iswdigit);
    CTESTGROUP(CNTRL, ISNOT, iswdigit);
    CTESTGROUP(DIGIT, IS, iswdigit);
    CTESTGROUP(GRAPH, ISNOT, iswdigit);
    CTESTGROUP(LC, ISNOT, iswdigit);
    CTESTGROUP(UC, ISNOT, iswdigit);
    CTESTGROUP(WS, ISNOT, iswdigit);
    CTESTGROUP(PUNCT, ISNOT, iswdigit);
    CTESTGROUP(PRINT, ISNOT, iswdigit);
    CTESTGROUP(XDIGIT, ISNOT, iswdigit);

    TEST_TRACE(C99 7.25.2.1.6)
    CTESTGROUP(ALL, ISNOT, iswgraph);
    CTESTGROUP(ALNUM, IS, iswgraph);
    CTESTGROUP(ALPHA, IS, iswgraph);
    CTESTGROUP(BLANK, ISNOT, iswgraph);
    CTESTGROUP(CNTRL, ISNOT, iswgraph);
    CTESTGROUP(DIGIT, IS, iswgraph);
    CTESTGROUP(GRAPH, IS, iswgraph);
    CTESTGROUP(LC, IS, iswgraph);
    CTESTGROUP(UC, IS, iswgraph);
    CTESTGROUP(WS, ISNOT, iswgraph);
    CTESTGROUP(PUNCT, IS, iswgraph);
    CTESTGROUP(PRINT, ISNOT, iswgraph);
    CTESTGROUP(XDIGIT, IS, iswgraph);

    TEST_TRACE(C99 7.25.2.1.7)
    CTESTGROUP(ALL, ISNOT, iswlower);
    CTESTGROUP(ALNUM, ISNOT, iswlower);
    CTESTGROUP(ALPHA, ISNOT, iswlower);
    CTESTGROUP(BLANK, ISNOT, iswlower);
    CTESTGROUP(CNTRL, ISNOT, iswlower);
    CTESTGROUP(DIGIT, ISNOT, iswlower);
    CTESTGROUP(GRAPH, ISNOT, iswlower);
    CTESTGROUP(LC, IS, iswlower);
    CTESTGROUP(UC, ISNOT, iswlower);
    CTESTGROUP(WS, ISNOT, iswlower);
    CTESTGROUP(PUNCT, ISNOT, iswlower);
    CTESTGROUP(PRINT, ISNOT, iswlower);
    CTESTGROUP(XDIGIT, ISNOT, iswlower);

    TEST_TRACE(C99 7.25.2.1.8)
    CTESTGROUP(ALL, ISNOT, iswprint);
    CTESTGROUP(ALNUM, IS, iswprint);
    CTESTGROUP(ALPHA, IS, iswprint);
    CTESTGROUP(BLANK, ISNOT, iswprint);
    CTESTGROUP(CNTRL, ISNOT, iswprint);
    CTESTGROUP(DIGIT, IS, iswprint);
    CTESTGROUP(GRAPH, IS, iswprint);
    CTESTGROUP(LC, IS, iswprint);
    CTESTGROUP(UC, IS, iswprint);
    CTESTGROUP(WS, ISNOT, iswprint);
    CTESTGROUP(PUNCT, IS, iswprint);
    CTESTGROUP(PRINT, IS, iswprint);
    CTESTGROUP(XDIGIT, IS, iswprint);
    
    TEST_TRACE(C99 7.25.2.1.9)
    CTESTGROUP(ALL, ISNOT, iswpunct);
    CTESTGROUP(ALNUM, ISNOT, iswpunct);
    CTESTGROUP(ALPHA, ISNOT, iswpunct);
    CTESTGROUP(BLANK, ISNOT, iswpunct);
    CTESTGROUP(CNTRL, ISNOT, iswpunct);
    CTESTGROUP(DIGIT, ISNOT, iswpunct);
    CTESTGROUP(GRAPH, ISNOT, iswpunct);
    CTESTGROUP(LC, ISNOT, iswpunct);
    CTESTGROUP(UC, ISNOT, iswpunct);
    CTESTGROUP(WS, ISNOT, iswpunct);
    CTESTGROUP(PUNCT, IS, iswpunct);
    CTESTGROUP(PRINT, ISNOT, iswpunct);
    CTESTGROUP(XDIGIT, ISNOT, iswpunct);

    TEST_TRACE(C99 7.25.2.1.10)
    CTESTGROUP(ALL, ISNOT, iswspace);
    CTESTGROUP(ALNUM, ISNOT, iswspace);
    CTESTGROUP(ALPHA, ISNOT, iswspace);
    CTESTGROUP(BLANK, IS, iswspace);
    CTESTGROUP(CNTRL, IS, iswspace);
    CTESTGROUP(DIGIT, ISNOT, iswspace);
    CTESTGROUP(GRAPH, ISNOT, iswspace);
    CTESTGROUP(LC, ISNOT, iswspace);
    CTESTGROUP(UC, ISNOT, iswspace);
    CTESTGROUP(WS, IS, iswspace);
    CTESTGROUP(PUNCT, ISNOT, iswspace);
    CTESTGROUP(PRINT, ISNOT, iswspace);
    CTESTGROUP(XDIGIT, ISNOT, iswspace);

    TEST_TRACE(C99 7.25.2.1.11)
    CTESTGROUP(ALL, ISNOT, iswupper);
    CTESTGROUP(ALNUM, ISNOT, iswupper);
    CTESTGROUP(ALPHA, ISNOT, iswupper);
    CTESTGROUP(BLANK, ISNOT, iswupper);
    CTESTGROUP(CNTRL, ISNOT, iswupper);
    CTESTGROUP(DIGIT, ISNOT, iswupper);
    CTESTGROUP(GRAPH, ISNOT, iswupper);
    CTESTGROUP(LC, ISNOT, iswupper);
    CTESTGROUP(UC, IS, iswupper);
    CTESTGROUP(WS, ISNOT, iswupper);
    CTESTGROUP(PUNCT, ISNOT, iswupper);
    CTESTGROUP(PRINT, ISNOT, iswupper);
    CTESTGROUP(XDIGIT, ISNOT, iswupper);

    TEST_TRACE(C99 7.25.2.1.12)
    CTESTGROUP(ALL, ISNOT, iswxdigit);
    CTESTGROUP(ALNUM, ISNOT, iswxdigit);
    CTESTGROUP(ALPHA, ISNOT, iswxdigit);
    CTESTGROUP(BLANK, ISNOT, iswxdigit);
    CTESTGROUP(CNTRL, ISNOT, iswxdigit);
    CTESTGROUP(DIGIT, IS, iswxdigit);
    CTESTGROUP(GRAPH, ISNOT, iswxdigit);
    CTESTGROUP(LC, ISNOT, iswxdigit);
    CTESTGROUP(UC, ISNOT, iswxdigit);
    CTESTGROUP(WS, ISNOT, iswxdigit);
    CTESTGROUP(PUNCT, ISNOT, iswxdigit);
    CTESTGROUP(PRINT, ISNOT, iswxdigit);
    CTESTGROUP(XDIGIT, IS, iswxdigit);

    TEST_TRACE(C99 7.25.2.2.1)
    TEST_EXCLUDE(PPC64, "http://ellcc.org/bugzilla/show_bug.cgi?id=39")
    {
        CTESTGROUP2(ALL, ISNOT, alnum);
        CTESTGROUP2(ALNUM, IS, alnum);
        CTESTGROUP2(ALPHA, IS, alnum);
        CTESTGROUP2(BLANK, ISNOT, alnum);
        CTESTGROUP2(CNTRL, ISNOT, alnum);
        CTESTGROUP2(DIGIT, IS, alnum);
        CTESTGROUP2(GRAPH, ISNOT, alnum);
        CTESTGROUP2(LC, IS, alnum);
        CTESTGROUP2(UC, IS, alnum);
        CTESTGROUP2(WS, ISNOT, alnum);
        CTESTGROUP2(PUNCT, ISNOT, alnum);
        CTESTGROUP2(PRINT, ISNOT, alnum);
        CTESTGROUP2(XDIGIT, IS, alnum);

        CTESTGROUP2(ALL, ISNOT, alpha);
        CTESTGROUP2(ALNUM, ISNOT, alpha);
        CTESTGROUP2(ALPHA, IS, alpha);
        CTESTGROUP2(BLANK, ISNOT, alnum);
        CTESTGROUP2(CNTRL, ISNOT, alnum);
        CTESTGROUP2(DIGIT, ISNOT, alpha);
        CTESTGROUP2(GRAPH, ISNOT, alpha);
        CTESTGROUP2(LC, IS, alpha);
        CTESTGROUP2(UC, IS, alpha);
        CTESTGROUP2(WS, ISNOT, alpha);
        CTESTGROUP2(PUNCT, ISNOT, alpha);
        CTESTGROUP2(PRINT, ISNOT, alpha);
        CTESTGROUP2(XDIGIT, ISNOT, alpha);

        CTESTGROUP2(ALL, ISNOT, blank);
        CTESTGROUP2(ALNUM, ISNOT, blank);
        CTESTGROUP2(ALPHA, ISNOT, blank);
        CTESTGROUP2(BLANK, IS, blank);
        CTESTGROUP2(CNTRL, ISNOT, blank);
        CTESTGROUP2(DIGIT, ISNOT, blank);
        CTESTGROUP2(GRAPH, ISNOT, blank);
        CTESTGROUP2(LC, ISNOT, blank);
        CTESTGROUP2(UC, ISNOT, blank);
        CTESTGROUP2(WS, ISNOT, blank);
        CTESTGROUP2(PUNCT, ISNOT, blank);
        CTESTGROUP2(PRINT, ISNOT, blank);
        CTESTGROUP2(XDIGIT, ISNOT, blank);

        CTESTGROUP2(ALL, ISNOT, cntrl);
        CTESTGROUP2(ALNUM, ISNOT, cntrl);
        CTESTGROUP2(ALPHA, ISNOT, cntrl);
        CTESTGROUP2(BLANK, ISNOT, cntrl);
        CTESTGROUP2(CNTRL, IS, cntrl);
        CTESTGROUP2(DIGIT, ISNOT, cntrl);
        CTESTGROUP2(GRAPH, ISNOT, cntrl);
        CTESTGROUP2(LC, ISNOT, cntrl);
        CTESTGROUP2(UC, ISNOT, cntrl);
        CTESTGROUP2(WS, ISNOT, cntrl);
        CTESTGROUP2(PUNCT, ISNOT, cntrl);
        CTESTGROUP2(PRINT, ISNOT, cntrl);
        CTESTGROUP2(XDIGIT, ISNOT, cntrl);

        CTESTGROUP2(ALL, ISNOT, digit);
        CTESTGROUP2(ALNUM, ISNOT, digit);
        CTESTGROUP2(ALPHA, ISNOT, digit);
        CTESTGROUP2(BLANK, ISNOT, digit);
        CTESTGROUP2(CNTRL, ISNOT, digit);
        CTESTGROUP2(DIGIT, IS, digit);
        CTESTGROUP2(GRAPH, ISNOT, digit);
        CTESTGROUP2(LC, ISNOT, digit);
        CTESTGROUP2(UC, ISNOT, digit);
        CTESTGROUP2(WS, ISNOT, digit);
        CTESTGROUP2(PUNCT, ISNOT, digit);
        CTESTGROUP2(PRINT, ISNOT, digit);
        CTESTGROUP2(XDIGIT, ISNOT, digit);

        CTESTGROUP2(ALL, ISNOT, graph);
        CTESTGROUP2(ALNUM, IS, graph);
        CTESTGROUP2(ALPHA, IS, graph);
        CTESTGROUP2(BLANK, ISNOT, graph);
        CTESTGROUP2(CNTRL, ISNOT, graph);
        CTESTGROUP2(DIGIT, IS, graph);
        CTESTGROUP2(GRAPH, IS, graph);
        CTESTGROUP2(LC, IS, graph);
        CTESTGROUP2(UC, IS, graph);
        CTESTGROUP2(WS, ISNOT, graph);
        CTESTGROUP2(PUNCT, IS, graph);
        CTESTGROUP2(PRINT, ISNOT, graph);
        CTESTGROUP2(XDIGIT, IS, graph);

        CTESTGROUP2(ALL, ISNOT, lower);
        CTESTGROUP2(ALNUM, ISNOT, lower);
        CTESTGROUP2(ALPHA, ISNOT, lower);
        CTESTGROUP2(BLANK, ISNOT, lower);
        CTESTGROUP2(CNTRL, ISNOT, lower);
        CTESTGROUP2(DIGIT, ISNOT, lower);
        CTESTGROUP2(GRAPH, ISNOT, lower);
        CTESTGROUP2(LC, IS, lower);
        CTESTGROUP2(UC, ISNOT, lower);
        CTESTGROUP2(WS, ISNOT, lower);
        CTESTGROUP2(PUNCT, ISNOT, lower);
        CTESTGROUP2(PRINT, ISNOT, lower);
        CTESTGROUP2(XDIGIT, ISNOT, lower);

        CTESTGROUP2(ALL, ISNOT, print);
        CTESTGROUP2(ALNUM, IS, print);
        CTESTGROUP2(ALPHA, IS, print);
        CTESTGROUP2(BLANK, ISNOT, print);
        CTESTGROUP2(CNTRL, ISNOT, print);
        CTESTGROUP2(DIGIT, IS, print);
        CTESTGROUP2(GRAPH, IS, print);
        CTESTGROUP2(LC, IS, print);
        CTESTGROUP2(UC, IS, print);
        CTESTGROUP2(WS, ISNOT, print);
        CTESTGROUP2(PUNCT, IS, print);
        CTESTGROUP2(PRINT, IS, print);
        CTESTGROUP2(XDIGIT, IS, print);
        
        CTESTGROUP2(ALL, ISNOT, punct);
        CTESTGROUP2(ALNUM, ISNOT, punct);
        CTESTGROUP2(ALPHA, ISNOT, punct);
        CTESTGROUP2(BLANK, ISNOT, punct);
        CTESTGROUP2(CNTRL, ISNOT, punct);
        CTESTGROUP2(DIGIT, ISNOT, punct);
        CTESTGROUP2(GRAPH, ISNOT, punct);
        CTESTGROUP2(LC, ISNOT, punct);
        CTESTGROUP2(UC, ISNOT, punct);
        CTESTGROUP2(WS, ISNOT, punct);
        CTESTGROUP2(PUNCT, IS, punct);
        CTESTGROUP2(PRINT, ISNOT, punct);
        CTESTGROUP2(XDIGIT, ISNOT, punct);

        CTESTGROUP2(ALL, ISNOT, space);
        CTESTGROUP2(ALNUM, ISNOT, space);
        CTESTGROUP2(ALPHA, ISNOT, space);
        CTESTGROUP2(BLANK, IS, space);
        CTESTGROUP2(CNTRL, IS, space);
        CTESTGROUP2(DIGIT, ISNOT, space);
        CTESTGROUP2(GRAPH, ISNOT, space);
        CTESTGROUP2(LC, ISNOT, space);
        CTESTGROUP2(UC, ISNOT, space);
        CTESTGROUP2(WS, IS, space);
        CTESTGROUP2(PUNCT, ISNOT, space);
        CTESTGROUP2(PRINT, ISNOT, space);
        CTESTGROUP2(XDIGIT, ISNOT, space);

        CTESTGROUP2(ALL, ISNOT, upper);
        CTESTGROUP2(ALNUM, ISNOT, upper);
        CTESTGROUP2(ALPHA, ISNOT, upper);
        CTESTGROUP2(BLANK, ISNOT, upper);
        CTESTGROUP2(CNTRL, ISNOT, upper);
        CTESTGROUP2(DIGIT, ISNOT, upper);
        CTESTGROUP2(GRAPH, ISNOT, upper);
        CTESTGROUP2(LC, ISNOT, upper);
        CTESTGROUP2(UC, IS, upper);
        CTESTGROUP2(WS, ISNOT, upper);
        CTESTGROUP2(PUNCT, ISNOT, upper);
        CTESTGROUP2(PRINT, ISNOT, upper);
        CTESTGROUP2(XDIGIT, ISNOT, upper);

        CTESTGROUP2(ALL, ISNOT, xdigit);
        CTESTGROUP2(ALNUM, ISNOT, xdigit);
        CTESTGROUP2(ALPHA, ISNOT, xdigit);
        CTESTGROUP2(BLANK, ISNOT, xdigit);
        CTESTGROUP2(CNTRL, ISNOT, xdigit);
        CTESTGROUP2(DIGIT, IS, xdigit);
        CTESTGROUP2(GRAPH, ISNOT, xdigit);
        CTESTGROUP2(LC, ISNOT, xdigit);
        CTESTGROUP2(UC, ISNOT, xdigit);
        CTESTGROUP2(WS, ISNOT, xdigit);
        CTESTGROUP2(PUNCT, ISNOT, xdigit);
        CTESTGROUP2(PRINT, ISNOT, xdigit);
        CTESTGROUP2(XDIGIT, IS, xdigit);
    }

    TEST_TRACE(C99 7.25.2.2.2)
    TEST(wctype("nonsense type") == 0, "wctype(nonsense type) == 0");

    TEST_TRACE(C99 7.25.3.1.1)
    CTESTMAP(UC, LC, towlower);
    CTESTMAP(ALLUC, ALLLC, towlower);
    TEST_TRACE(C99 7.25.3.1.2)
    CTESTMAP(LC, UC, towupper);
    CTESTMAP(ALLLC, ALLUC, towupper);

    TEST_RESOLVED(ALL_PROCESSORS, "http://ellcc.org/bugzilla/show_bug.cgi?id=40")
    {
        TEST_TRACE(C99 7.25.3.2.1)
        CTESTMAP2(UC, LC, tolower);
        CTESTMAP2(ALLUC, ALLLC, tolower);
        CTESTMAP2(LC, UC, toupper);
        CTESTMAP2(ALLLC, ALLUC, toupper);
    }
    TEST_TRACE(C99 7.25.3.2.2)
    TEST(wctrans("nonsense trans") == 0, "wctrans(nonsense trans) == 0");
    TEST(wctrans("tolower") != 0, "wctrans(tolower) != 0");
END_GROUP

