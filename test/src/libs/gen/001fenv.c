#include <ecc_test.h>
#include <fenv.h>

TEST_GROUP(Fenv)
    // C99 7.6
    #pragma FENV_ACCESS ON
    fenv_t fenv = *FE_DFL_ENV;
    fexcept_t fexcept = FE_ALL_EXCEPT;
    int i;
    // C99 7.6.2.1
    feclearexcept(FE_ALL_EXCEPT);
    // C99 7.6.2.2
    fegetexceptflag(&fexcept, FE_ALL_EXCEPT);
    // C99 7.6.2.3
    feraiseexcept(FE_ALL_EXCEPT);
    // C99 7.6.2.4
    fesetexceptflag(&fexcept, FE_ALL_EXCEPT);
    // C99 7.6.2.5
    i = fetestexcept(FE_ALL_EXCEPT);
    // C99 7.6.3.1
    i = fegetround();
    // C99 7.6.3.2
    i = fesetround(0);
    // C99 7.6.4.1
    fegetenv(&fenv);
    // C99 7.6.4.2
    i = feholdexcept(&fenv);
    // C99 7.6.4.3
    fesetenv(&fenv);
    // C99 7.6.4.4
    feupdateenv(&fenv);
END_GROUP

