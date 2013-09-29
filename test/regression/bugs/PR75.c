// XFAIL: *
// RUN: %mipsecc -o %t %s -lm && %mipsrun %t
// RUN: %mipselecc -o %t %s -lm && %mipselrun %t
#include <math.h>
#include <limits.h>

int main()
{
    isnan(1.0);
    if (isnormal(0.0)) exit (1);
}
