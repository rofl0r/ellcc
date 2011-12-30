#include <ecc_test.h>
#include <fenv.h>

TEST_GROUP(Fenv)
    // 7.6
    #pragma FENV_ACCESS ON
    fenv_t fenv = *FE_DFL_ENV;
    fexcept_t fexcept = FE_ALL_EXCEPT;
    int i;
    // 7.6.2.1
    feclearexcept(FE_ALL_EXCEPT);
    // 7.6.2.2
    fegetexceptflag(&fexcept, FE_ALL_EXCEPT);
    // 7.6.2.3
    feraiseexcept(FE_ALL_EXCEPT);
    // 7.6.2.4
    fesetexceptflag(&fexcept, FE_ALL_EXCEPT);
    // 7.6.2.5
    i = fetestexcept(FE_ALL_EXCEPT);
    // 7.6.3.1
    i = fegetround();
    // 7.6.3.2
    i = fesetround(0);
    // 7.6.4.1
    fegetenv(&fenv);
    // 7.6.4.2
    i = feholdexcept(&fenv);
    // 7.6.4.3
    fesetenv(&fenv);
    // 7.6.4.4
    feupdateenv(&fenv);
END_GROUP

