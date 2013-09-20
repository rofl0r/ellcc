// RUN: %ecc -o %t %s && %t | FileCheck -check-prefix=CHECK %s
// RUN: %armecc -o %t %s && %armrun %t | FileCheck -check-prefix=CHECK %s
// RUN: %armebecc -o %t %s && %armebrun %t | FileCheck -check-prefix=CHECK %s
// RUN: %i386ecc -o %t %s && %i386run %t | FileCheck -check-prefix=CHECK %s
// RUN: %microblazeecc -o %t %s && %microblazerun %t | FileCheck -check-prefix=CHECK %s
// RUN: %mipsecc -o %t %s && %mipsrun %t | FileCheck -check-prefix=CHECK %s
// RUN: %mipselecc -o %t %s && %mipselrun %t | FileCheck -check-prefix=CHECK %s
// RUN: %ppcecc -o %t %s && %ppcrun %t | FileCheck -check-prefix=CHECK %s
// FAIL: %ppc64ecc -o %t %s && %ppc64run %t | FileCheck -check-prefix=CHECK %s
// RUN: %x86_64ecc -o %t %s && %x86_64run %t | FileCheck -check-prefix=CHECK %s
// CHECK: hello world
#include <stdio.h>
int main(int argc, char **argv)
{
    printf("hello world\n");
}
