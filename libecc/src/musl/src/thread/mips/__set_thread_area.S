#include <bits/asm.h>
#include <bits/syscall.h>
        .text
        .global __set_thread_area
        .type   __set_thread_area,@function
__set_thread_area:
        .set    noreorder
        li      v0, __NR_set_thread_area
#if defined(__mips_o32)
        addi    sp, sp, -(4*8)
#endif
        syscall
#if defined(__mips_o32)
        addi    sp, sp, (4*8)
#endif
        .set    reorder
        jr      ra
        nop
