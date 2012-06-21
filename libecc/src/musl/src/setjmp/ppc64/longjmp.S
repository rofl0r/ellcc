#include <bits/asm.h>
        .global _longjmp
        .global longjmp
        .type   _longjmp,@function
        .type   longjmp,@function
_longjmp:
longjmp:
        cmpi    r4, 0, r3, 0
        beq     r4, 1f
        addi    r3, r3, 1
1:      lmw     r8, 4(r3)               // load r8-r31
        mr      r6, r4
        mtlr    r11
        mtcr    r12
        mr      r2, r9
        mr      r1, r10
        blr 
