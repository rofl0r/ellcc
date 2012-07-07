#include <bits/syscall.h>
        .global __restore
        .type __restore,@function
__restore:
        ori     r12, r0, __NR_sigreturn
        brki    r14, 0x8                // syscall

        .global __restore_rt
        .type __restore_rt,@function
__restore_rt:
        ori     r12, r0, __NR_rt_sigreturn
        brki    r14, 0x8                // syscall
