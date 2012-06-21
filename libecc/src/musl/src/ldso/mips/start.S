#include <bits/asm.h>
        .global _start
        .type   _start,@function
        .align  2
        .ent    _start
_start:
        .mask   0x00000000,0
        .fmask  0x00000000,0
        subu    fp, fp, fp              // Zero the frame pointer.
        la      gp, _gp
        lw      a1, (sp)                // Get argc...
        addu    a2, sp, 4               // ... and argv ...
#if defined(__mips_o32)
        addi    sp, sp, -4*4            // Leave space for arguments 0..3.
#endif
        // Get the musl dynamic linker entry point.
        lw      t0, %call16(__dynlink)(gp)
        jalr    t0                      // Let's go!
        nop
#if defined(__mips_o32)
        addi    sp, sp, 4*4             // Leave space for arguments 0..3.
#endif
        li      t0, 1
        beq     v0, t0, .               // Stay here.
        nop
        li      a0, 0
        jr      v0                      // Otherwise go.
        nop
        .end    _start
        .size   _start, .-_start

