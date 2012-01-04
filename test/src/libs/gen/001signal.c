#include <ecc_test.h>
#include <signal.h>

// C99 7.14/2
static volatile sig_atomic_t i = 0;

static void handler(int sig)
{
    i = 1;
}

TEST_GROUP(Signal)
    // C99 7.14/3
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
    TEST(SIGTERM > 0, "SIGSEGV is a positive value");
    // C99 7.14.1.1
    signal(SIGINT, handler);
    // C99 7.14.1.1/8
#if !defined(__microblaze__)
    TEST(signal(SIGINT, handler) == handler, "The previous signal is correct");
#endif
    // C99 7.14.1.2
    // RICH:
#if !defined(__arm__) && !defined(__microblaze__) && !defined(__ppc64__) && !defined(__x86_64__)
    TEST(raise(SIGINT) == 0, "Raise a signal");
    TEST(i == 1, "The signal handler has been called");
#endif
END_GROUP

