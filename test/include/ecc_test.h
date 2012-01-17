/** @file
 * This file defines ECC test harness definitions.
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

extern int __test_count;                      ///< The test counter.
extern int __test_failures;                   ///< The failure counter.
extern int __test_verbose;                    ///< The test verbosity control.
extern const char *__test_cases;              ///< The current test cases.
extern const char *__test_category;           ///< The current test category.
extern const char *__test_group;              ///< The current test group.

/** Define an existance macro for each processor.
 */
#if defined(__arm__)
#define ARM 1
#else
#define ARM 0
#endif
#if !defined(__clang__)
#define GCC 1
#else
#define GCC 0
#endif
#if defined(__host__)
#define HOST 1
#else
#define HOST 0
#endif
#if defined(__i386__)
#define I386 1
#else
#define I386 0
#endif
#if defined(__microblaze__)
#define MICROBLAZE 1
#else
#define MICROBLAZE 0
#endif
#if defined(__mips__)
#define MIPS 1
#else
#define MIPS 0
#endif
#if defined(__nios2__)
#define NIOS2 1
#else
#define NIOS2 0
#endif
#if defined(__ppc64__)
#define PPC64 1
#define PPC 0
#else
#define PPC64 0
  #if defined(__ppc__)
  #define PPC 1
  #else
  #define PPC 0
  #endif
#endif
#if defined(__sparc__)
#define SPARC 1
#else
#define SPARC 0
#endif
#if defined(__x86_64__)
#define X86_64 1
#else
#define X86_64 0
#endif

/** All known processors
 */
#define ALL_PROCESSORS \
    (ARM + GCC + I386 + MICROBLAZE + MIPS + NIOS2 + PPC + PPC64 + SPARC + X86_64)

/** Check for exactly one processor definition.
 */
#if ALL_PROCESSORS == 0
#error Unknown processor
#endif
#if ALL_PROCESSORS != 1
  #if !GCC
  #error Multiple processor definitions
  #endif
#endif

/** Initialize the test harness.
 */
#define TEST_INIT(name, verbose)                                        \
int __test_count;                                                       \
int __test_failures;                                                    \
int __test_verbose;                                                     \
const char *__test_cases = "";                                          \
const char *__test_category = "";                                       \
const char *__test_group = "";                                          \
static void done(void) { TEST_DONE(); }                                 \
static void finish(void) __attribute__((__constructor__, __used__));    \
static void finish(void) { atexit(done); }                              \
int main(int argc, char **argv)                                         \
{                                                                       \
    __test_verbose = verbose;                                           \
    __test_category = #name;                                            \
}

/** Define test cases.
 */
#define TEST_GROUP(which)                                               \
static void test ## which(void)                                         \
    __attribute__((__destructor__, __used__));                          \
static void test ## which(void) {                                       \
    errno = 0;                                                          \
    __test_group = #which;                                              \
    if (__test_verbose) {                                               \
        fprintf(stdout, "%s Test Group " #which " in %s\n", __test_category, __FILE__); }
#define END_GROUP }

/** Set the test category.
 */
#define TEST_CATEGORY(name)                                             \
    do {                                                                \
        __test_category = #name;                                        \
    } while (0)

/** Set a test reference trace.
 */
#define TEST_TRACE(ref)

/** Define a test exclusion set.
 */
#define TEST_EXCLUDE(set, bug) if (!(set))

/** Define an unsupported test.
 */
#define TEST_UNSUPPORTED(x)

/** Perform a test.
 */
#define TEST(cond, ...)                                                 \
    do {                                                                \
        ++__test_count;                                                 \
        const char *file = strrchr(__FILE__, '/');                      \
        if (file == NULL) file = __FILE__; else ++file;                 \
        if (!(cond)) {                                                  \
            fprintf(stdout, "FAIL: %s:%d: %s(%s): ", file, __LINE__,    \
                    __test_category, __test_group);                     \
            fprintf(stdout, __VA_ARGS__);                               \
            fprintf(stdout, "\n");                                      \
            fprintf(stderr, "FAIL: %s:%d: %s(%s): ", file, __LINE__,    \
                    __test_category, __test_group);                     \
            fprintf(stderr, __VA_ARGS__);                               \
            fprintf(stderr, "\n");                                      \
            ++__test_failures;                                          \
        } else if (__test_verbose) {                                    \
            fprintf(stdout, "PASS: %s:%d: %s(%s): ", file, __LINE__,    \
                    __test_category, __test_group);                     \
            fprintf(stdout, __VA_ARGS__);                               \
            fprintf(stdout, "\n");                                      \
        }                                                               \
    } while (0)

/** Complete testing.
 */
#define TEST_DONE()                                                     \
    do {                                                                \
        fprintf(stdout, "%s unit tests completed\n", __test_category);  \
        fprintf(stdout, "    %d tests run\n", __test_count);            \
        fprintf(stdout, "    %d test%s failed\n", __test_failures,      \
                __test_failures == 1 ? "" : "s");                       \
        if (__test_failures > 0) {                                      \
            fprintf(stderr, "%s unit tests completed\n", __FILE__);     \
            fprintf(stderr, "    %d tests run\n", __test_count);        \
            fprintf(stderr, "    %d test%s failed\n", __test_failures,  \
                __test_failures == 1 ? "" : "s");                       \
            exit(EXIT_FAILURE);                                         \
        } else {                                                        \
            exit(EXIT_SUCCESS);                                         \
        }                                                               \
    } while (0)

#endif // _ecc_test_h_
