// RUN: %ecc --target=x86_64-ellcc-linux %s && ./a.out | FileCheck -check-prefix=CHECK %s
// CHECK: hello world
#include <stdio.h>
int main(int argc, char **argv)
{
    // printf("hello world\n");
}
