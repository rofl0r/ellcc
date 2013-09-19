// Compile for every target.
// RUN: %ecc -o %t %s -lm
// RUN: %armecc -o %t %s -lm
// RUN: %armebecc -o %t %s -lm
// RUN: %i386ecc -o %t %s -lm
// RUN: %microblazeecc -o %t %s -lm
// RUN: %mipsecc -o %t %s -lm
// RUN: %mipselecc -o %t %s -lm
// RUN: %ppcecc -o %t %s -lm
// FAIL: %ppc64ecc -o %t %s -lm
// RUN: %x86_64ecc -o %t %s -lm
#include "../ecc_test.h"
#include <fenv.h>

TEST_GROUP(Fenv)
    TEST_TRACE(C99 7.6)
    #pragma FENV_ACCESS ON
    const fenv_t *fenvp = FE_DFL_ENV;
    fenv_t fenv;
    fexcept_t fexcept = FE_ALL_EXCEPT;
    int i;
    TEST_TRACE(C99 7.6.2.1)
    feclearexcept(FE_ALL_EXCEPT);
    TEST_TRACE(C99 7.6.2.2)
    fegetexceptflag(&fexcept, FE_ALL_EXCEPT);
    TEST_TRACE(C99 7.6.2.3)
    feraiseexcept(FE_ALL_EXCEPT);
    TEST_TRACE(C99 7.6.2.4)
    fesetexceptflag(&fexcept, FE_ALL_EXCEPT);
    TEST_TRACE(C99 7.6.2.5)
    i = fetestexcept(FE_ALL_EXCEPT);
    TEST_TRACE(C99 7.6.3.1)
    i = fegetround();
    TEST_TRACE(C99 7.6.3.2)
    i = fesetround(0);
    TEST_TRACE(C99 7.6.4.1)
    fegetenv(&fenv);
    TEST_TRACE(C99 7.6.4.2)
    i = feholdexcept(&fenv);
    TEST_TRACE(C99 7.6.4.3)
    fesetenv(&fenv);
    TEST_TRACE(C99 7.6.4.4)
    feupdateenv(&fenv);
END_GROUP

