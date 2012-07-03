#include <bits/asm.h>
        .global sigsetjmp
        .type sigsetjmp,@function
sigsetjmp:
        lwz     r4, 64*8-2*8(r3)        // Second last long.
        cmpi    r4, 0, r4, 0
        bne     r4, 1f
        addi    r5, r3, 64*8-1*8        // Address of last long.
        li      r4, 0
        li      r3, 2
        bl      sigprocmask
1:      b       setjmp
