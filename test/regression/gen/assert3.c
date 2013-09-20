// Compile and run for every target.
// RUN: %ecc -o %t %s && not --crash %t
// RUN: %armecc -o %t %s && not --crash %armrun %t && rm -f *.core
// RUN: %armebecc -o %t %s && not --crash %armebrun %t && rm -f *.core
// RUN: %i386ecc -o %t %s && not --crash %i386run %t && rm -f *.core
// RUN: %microblazeecc -o %t %s && not --crash %microblazerun %t && rm -f *.core
// RUN: %mipsecc -o %t %s && not --crash %mipsrun %t && rm -f *.core
// RUN: %mipselecc -o %t %s && not --crash %mipselrun %t && rm -f *.core
// RUN: %ppcecc -o %t %s && not --crash %ppcrun %t && rm -f *.core
// FAIL: %ppc64ecc -o %t %s && not --crash %ppc64run %t && rm -f *.core
// RUN: %x86_64ecc -o %t %s && not --crash %x86_64run %t && rm -f *.core
// TEST_TRACE(C99 7.2)
#include <assert.h>
int main()
{
    assert(0);
}
