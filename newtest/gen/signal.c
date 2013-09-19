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
#include <signal.h>

TEST_TRACE(C99 7.14/2)
static volatile sig_atomic_t i = 0;

static void handler(int sig)
{
    fprintf(stderr, "in handler\n");
    i = 1;
}

TEST_GROUP(Signal)
    TEST_TRACE(C99 7.14/3)
    TEST(SIG_DFL != SIG_ERR && SIG_DFL != SIG_IGN,
         "SIG_DLF has a distinct value");
    TEST(SIG_ERR != SIG_IGN,
         "SIG_ERR has a distinct value");
    
    TEST(SIGABRT > 0, "SIGABRT is a positive value");
    TEST(SIGABRT != SIGFPE && SIGABRT != SIGILL
      && SIGABRT != SIGINT && SIGABRT != SIGSEGV
      && SIGABRT != SIGTERM,
         "SIGABRT has a distinct value");
    TEST(SIGFPE > 0, "SIGFPE is a positive value");
    TEST(SIGFPE != SIGILL && SIGFPE != SIGINT
      && SIGFPE != SIGSEGV && SIGFPE != SIGTERM,
         "SIGFPE has a distinct value");
    TEST(SIGILL > 0, "SIGILL is a positive value");
    TEST(SIGILL != SIGINT && SIGILL != SIGSEGV
      && SIGILL != SIGTERM,
         "SIGILL has a distinct value");
    TEST(SIGINT > 0, "SIGINT is a positive value");
    TEST(SIGINT != SIGSEGV && SIGINT != SIGTERM,
         "SIGINT has a distinct value");
    TEST(SIGSEGV > 0, "SIGSEGV is a positive value");
    TEST(SIGSEGV != SIGTERM,
         "SIGSEGV has a distinct value");
    TEST(SIGTERM > 0, "SIGTERM is a positive value");
    TEST_TRACE(C99 7.14.1.1)
    signal(SIGINT, handler);
    TEST_TRACE(C99 7.14.1.1/8)
    TEST_RESOLVED(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=24")
        TEST(signal(SIGINT, handler) == handler, "The previous signal is correct");
    TEST_TRACE(C99 7.14.1.2)
    TEST_RESOLVED(MIPS, "http://ellcc.org/bugzilla/show_bug.cgi?id=55")
    TEST_RESOLVED(ARM, "http://ellcc.org/bugzilla/show_bug.cgi?id=10")
    TEST_RESOLVED(I386, "http://ellcc.org/bugzilla/show_bug.cgi?id=16")
    TEST_RESOLVED(MICROBLAZE, "http://ellcc.org/bugzilla/show_bug.cgi?id=11")
    TEST_EXCLUDE(PPC64, "http://ellcc.org/bugzilla/show_bug.cgi?id=12")
    TEST_EXCLUDE(X86_64, "http://ellcc.org/bugzilla/show_bug.cgi?id=13")
    {
        TEST(raise(SIGINT) == 0, "Raise a signal");
        TEST(i == 1, "The signal handler has been called");
    }
END_GROUP

