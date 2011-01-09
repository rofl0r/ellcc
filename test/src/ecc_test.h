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

static int unittest_count;                          ///< The test counter.
static int unittest_failures;                       ///< The failure counter.
static int unittest_verbose;                        ///< The test verbosity control.
static const char *unittest_category = "";          ///< The current test category.

/** Initialize the test harness.
 */
#define TEST_INIT(verbose)                                              \
    do {                                                                \
        unittest_verbose = verbose;                                     \
    } while (0)

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
        if (!(cond)) {                                                  \
            fprintf(stdout, "FAIL: %s:%d: %s: ", __FILE__, __LINE__,    \
                    unittest_category);                                 \
            fprintf(stdout, __VA_ARGS__);                               \
            fprintf(stdout, "\n");                                      \
            fprintf(stderr, "FAIL: %s:%d: %s: ", __FILE__, __LINE__,    \
                    unittest_category);                                 \
            fprintf(stderr, __VA_ARGS__);                               \
            fprintf(stderr, "\n");                                      \
            ++unittest_failures;                                        \
        } else if (unittest_verbose) {                                  \
            fprintf(stdout, "PASS: %s:%d: %s: ", __FILE__, __LINE__,    \
                    unittest_category);                                 \
            fprintf(stdout, __VA_ARGS__);                               \
            fprintf(stdout, "\n");                                      \
        }                                                               \
    } while (0)

/** Complete testing.
 */
#define TEST_DONE()                                                     \
    do {                                                                \
        fprintf(stdout, "%s unit tests completed\n", __FILE__);         \
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
