#include <bits/asm.h>
        .global sigsetjmp
        .type sigsetjmp,@function
sigsetjmp:
        lwz     r4, 48(r3)
        cmpi    r4, 0, r4, 0
        bne     r4, 1f
        addi    r5, r3, 52
        li      r4, 0
        li      r3, 2
        bl      sigprocmask
1:      b       setjmp
