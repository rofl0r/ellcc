#include <bits/asm.h>
        .global __syscall
        .align  2
        .type   __syscall,@function
        .ent    __syscall
__syscall:
        .set    noreorder
        move    v0, a0                  // Save the system call number
        move    a0, a1                  // Shift the arguments, arg1
        move    a1, a2                  // arg2
        move    a2, a3                  // arg3
#if defined(__mips_n64)
#error The 64 bit Mips processor is not supported.
#elif defined(__mips_n32)
        move    a3, a4                  // arg4
        move    a4, a5                  // arg5
        move    a5, a6                  // arg6
#else /* __mips_o32 */
        // Get arg4, arg5, and arg6.
        lw      a3, (4*4)+(4*0)(sp)
        lw      t0, (4*4)+(4*1)(sp)
        lw      t1, (4*4)+(4*2)(sp)
        addi    sp, sp, -(4*8)
        sw      t0, (4*4)+(4*0)(sp)
        sw      t1, (4*4)+(4*1)(sp)
#endif
        syscall
#if defined(__mips_o32)
        addi    sp, sp, (4*8)
        .set    reorder
#endif
        beq     a3, zero, 1f
        nop
        subu    v0, zero, v0
1:
	jr      ra
        nop
        .end    __syscall
        .size   __syscall, .-__syscall
