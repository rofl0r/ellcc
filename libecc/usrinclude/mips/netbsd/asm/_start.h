#include <sys/elf_note.h>

/** The initial entry point.
 */
asm(
"       .text\n"
"       .globl  _start\n"
"       .align  2\n"
"       .ent    _start\n"
"_start:\n"
"       .mask   0x00000000,0\n"
"       .fmask  0x00000000,0\n"
"       subu    $fp, $fp, $fp\n"
"       la      $gp, _gp\n"
"       nop\n"
"       lw      $4, ($sp)\n"            // Get argc...
"       nop\n"
"       addu    $5, $sp, 4\n"           // ... and argv ...
"       addi    $6, $4, 1\n"            // $6 = argc + 1
"       addu    $6, $6, $6\n"
"       addu    $6, $6, $6\n"           // ... $6 = argc * 4 ...
"       addu    $6, $6, $5\n"           // ... and compute environ.
"       lw      $25, %call16(_estart)($gp)\n"
"       jalr    $25\n"                  // Let's go!
"       nop\n"
"       b       .\n"                    // Never gets here.
"       nop\n"
"       .end    _start\n"
"       .size   _start, .-_start\n"
);

