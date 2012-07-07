#include <bits/asm.h>
        .global sigsetjmp
        .type sigsetjmp,@function
sigsetjmp:
        lw      t0, 64*4-2*4(a0)        // Second last long.
        beq     t0, zero, 1f
        addi    a2, a0, 64*4-1*4        // Address of last long.
        move    a1, zero
        li      a0, 2
#if defined(__mips_o32)
        addi    sp, sp, -(4*8)
#endif
        lw      t0, %call16(sigprocmask)(gp)
	jr      t0
#if defined(__mips_o32)
        addi    sp, sp, (4*8)
#endif
1:      lw      t0, %call16(setjmp)(gp)
	jr      t0
