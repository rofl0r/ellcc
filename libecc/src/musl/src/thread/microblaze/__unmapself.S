#include <bits/syscall.h>
        .text
        .global __unmapself
        .type   __unmapself,%function
__unmapself:
        ori     r12, r0, __NR_munmap
        brki    r14, 0x8                // syscall
        ori     r12, r0, __NR_exit
        brki    r14, 0x8                // syscall
        rtsd    r15, 8
        nop
