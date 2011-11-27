/** The initial entry point.
 */
        .text
        .globl  _start
        .align  2
        .ent    _start
_start:
        .mask   0x00000000,0
        .fmask  0x00000000,0
        subu    $fp, $fp, $fp
        la      $gp, _gp
        nop
        lw      $4, ($sp)               // Get argc...
        nop
        addu    $5, $sp, 4              // ... and argv ...
        addi    $6, $4, 1               // $6 = argc + 1
        addu    $6, $6, $6
        addu    $6, $6, $6              // ... $6 = argc * 4 ...
        addu    $6, $6, $5              // ... and compute environ.
        lw      $25, %call16(_estart)($gp)
        jalr    $25                     // Let's go!
        nop
        b       .                       // Never gets here.
        nop
        .end    _start
        .size   _start, .-_start
