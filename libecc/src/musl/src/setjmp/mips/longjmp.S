#include <bits/asm.h>
        .global _longjmp
        .global longjmp
        .type   _longjmp,@function
        .type   longjmp,@function
        .align  2
_longjmp:
longjmp:
        move    v0, a1
        bne     v0, zero, 1f
        addi    v0, 1
1:      lw      ra,  0(a0)
        lw      v0,  4(a0)
        lw      s0,  8(a0)
        lw      s1, 12(a0)
        lw      s2, 16(a0)
        lw      s3, 20(a0)
        lw      s4, 24(a0)
        lw      s5, 28(a0)
        lw      s6, 32(a0)
        lw      s7, 36(a0)
        lw      s8, 40(a0)
        lw      sp, 44(a0)
#if defined(__mips_n32)
        lw      gp, 48(a0)
#endif
	jr      ra
        nop
