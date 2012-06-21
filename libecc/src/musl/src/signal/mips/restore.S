#include <bits/asm.h>
#include <bits/syscall.h>
        .global __restore
        .type __restore,@function
__restore:
        .set    noreorder
        li      v0, __NR_sigreturn
        syscall
        .set    reorder

        .global __restore_rt
        .type __restore_rt,@function
__restore_rt:
        .set    noreorder
        li      v0, __NR_rt_sigreturn
        syscall
        .set    reorder
