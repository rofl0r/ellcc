#include <machine/regdef.h>

/** The initial entry point.
 */
        .text
        .globl  _start
        .align  2
        .ent    _start
_start:
        .mask   0x00000000,0
        .fmask  0x00000000,0
        subu    fp, fp, fp              // Zero the frame pointer
        la      gp, _gp
        nop
        lw      a0, (sp)                // Get argc...
        nop
        addu    a1, sp, 4               // ... and argv ...
        addi    a2, a0, 1               // a2 = argc + 1
        addu    a2, a2, a2
        addu    a2, a2, a2              // ... a2 = argc * 4 ...
        addu    a2, a2, a1              // ... and compute environ.
        addi    sp, sp, 16              // Leave space for arguments 0..3 (o32 abi).
        lw      t9, %call16(_estart)(gp)
        jalr    t9                      // Let's go!
        nop
        b       .                       // Never gets here.
        nop
        .end    _start
        .size   _start, .-_start
