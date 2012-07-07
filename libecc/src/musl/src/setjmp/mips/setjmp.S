#include <bits/asm.h>
        .global __setjmp
        .global _setjmp
        .global setjmp
        .type   __setjmp,@function
        .type   _setjmp,@function
        .type   setjmp,@function
        .align  2
__setjmp:
_setjmp:
setjmp:
        sw      ra,  0(a0)
        sw      v0,  4(a0)
        sw      s0,  8(a0)
        sw      s1, 12(a0)
        sw      s2, 16(a0)
        sw      s3, 20(a0)
        sw      s4, 24(a0)
        sw      s5, 28(a0)
        sw      s6, 32(a0)
        sw      s7, 36(a0)
        sw      s8, 40(a0)
        sw      sp, 44(a0)
#if defined(__mips_n32)
        sw      gp, 48(a0)
#endif
        li      v0, 0
	jr      ra
        nop
