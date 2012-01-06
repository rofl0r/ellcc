#include <ecc_test.h>
#include <signal.h>

TEST_TRACE(C99 7.14/2)
static volatile sig_atomic_t i = 0;

static void handler(int sig)
{
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
    TEST(SIGTERM > 0, "SIGSEGV is a positive value");
    TEST_TRACE(C99 7.14.1.1)
    signal(SIGINT, handler);
    TEST_TRACE(C99 7.14.1.1/8)
    TEST_EXCLUDE(MICROBLAZE)
        TEST(signal(SIGINT, handler) == handler, "The previous signal is correct");
    TEST_TRACE(C99 7.14.1.2)
    TEST_EXCLUDE(ARM|I386|MICROBLAZE|PPC64|X86_64) {
        TEST(raise(SIGINT) == 0, "Raise a signal");
        TEST(i == 1, "The signal handler has been called");
    }
END_GROUP

