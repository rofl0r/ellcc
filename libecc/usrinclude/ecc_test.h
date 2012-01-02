/** @file
 * This file defines ECC unit test definitions.
 *
 * @author Richard Pennington
 * @date Sun Jan  9 09:56:06 CST 2011
 */

#ifndef _ecc_test_h_
#define _ecc_test_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int unittest_count;                      ///< The test counter.
extern int unittest_failures;                   ///< The failure counter.
extern int unittest_verbose;                    ///< The test verbosity control.
extern const char *unittest_cases;              ///< The current test cases.
extern const char *unittest_category;           ///< The current test category.
extern const char *unittest_group;              ///< The current test group.

/** Initialize the test harness.
 */
#define TEST_INIT(name, verbose)                                        \
int unittest_count;                                                     \
int unittest_failures;                                                  \
int unittest_verbose;                                                   \
const char *unittest_cases = "";                                        \
const char *unittest_category = "";                                     \
const char *unittest_group = "";                                        \
static void done(void) { TEST_DONE(); }                                 \
static void finish(void) __attribute__((__constructor__, __used__));    \
static void finish(void) { atexit(done); }                              \
int main(int argc, char **argv)                                         \
{                                                                       \
    unittest_verbose = verbose;                                         \
    unittest_category = #name;                                          \
}

/** Define test cases.
 */
#define TEST_GROUP(which)                                               \
static void test ## which(void)                                         \
    __attribute__((__destructor__, __used__));                          \
static void test ## which(void) {                                       \
    errno = 0;                                                          \
    unittest_group = #which;                                            \
    if (unittest_verbose) {                                             \
        fprintf(stdout, "%s Test Group " #which " in %s\n", unittest_category, __FILE__); }
#define END_GROUP }

/** Set the test category.
 */
#define TEST_CATEGORY(name)                                             \
    do {                                                                \
        unittest_category = #name;                                      \
    } while (0)

/** Perform a test.
 */
#define TEST(cond, ...)                                                 \
    do {                                                                \
        ++unittest_count;                                               \
        const char *file = strrchr(__FILE__, '/');                      \
        if (file == NULL) file = __FILE__; else ++file;                 \
        if (!(cond)) {                                                  \
            fprintf(stdout, "FAIL: %s:%d: %s(%s): ", file, __LINE__,    \
                    unittest_category, unittest_group);                 \
            fprintf(stdout, __VA_ARGS__);                               \
            fprintf(stdout, "\n");                                      \
            fprintf(stderr, "FAIL: %s:%d: %s(%s): ", file, __LINE__,    \
                    unittest_category, unittest_group);                 \
            fprintf(stderr, __VA_ARGS__);                               \
            fprintf(stderr, "\n");                                      \
            ++unittest_failures;                                        \
        } else if (unittest_verbose) {                                  \
            fprintf(stdout, "PASS: %s:%d: %s(%s): ", file, __LINE__,    \
                    unittest_category, unittest_group);                 \
            fprintf(stdout, __VA_ARGS__);                               \
            fprintf(stdout, "\n");                                      \
        }                                                               \
    } while (0)

/** Complete testing.
 */
#define TEST_DONE()                                                     \
    do {                                                                \
        fprintf(stdout, "%s unit tests completed\n", unittest_category);\
        fprintf(stdout, "    %d tests run\n", unittest_count);          \
        fprintf(stdout, "    %d test%s failed\n", unittest_failures,    \
                unittest_failures == 1 ? "" : "s");                     \
        if (unittest_failures > 0) {                                    \
            fprintf(stderr, "%s unit tests completed\n", __FILE__);     \
            fprintf(stderr, "    %d tests run\n", unittest_count);      \
            fprintf(stderr, "    %d test%s failed\n", unittest_failures,\
                unittest_failures == 1 ? "" : "s");                     \
            exit(EXIT_FAILURE);                                         \
        } else {                                                        \
            exit(EXIT_SUCCESS);                                         \
        }                                                               \
    } while (0)

#endif // _ecc_test_h_
