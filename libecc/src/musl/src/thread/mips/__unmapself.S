#include <bits/asm.h>
#include <bits/syscall.h>
        .text
        .global __unmapself
        .type   __unmapself,%function
__unmapself:
        .set    noreorder
#if defined(__mips_o32)
        addi    sp, sp, -(4*8)
#endif
        li      v0, __NR_munmap
        syscall
        li      v0, __NR_exit
        syscall
#if defined(__mips_o32)
        addi    sp, sp, (4*8)
#endif
        .set    reorder
        jr      ra
        nop

