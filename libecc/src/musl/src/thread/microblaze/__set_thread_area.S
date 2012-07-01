#include <bits/syscall.h>
        .text
        .global __set_thread_area
        .type   __set_thread_area,@function
__set_thread_area:
        ori     r12, r0, __NR_set_thread_area
        brki    r14, 0x8                // syscall
        rtsd    r15, 8
        nop
