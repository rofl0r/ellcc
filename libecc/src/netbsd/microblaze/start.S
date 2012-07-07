#include <sys/elf_note.h>

/** The initial entry point.
 */
        .text
        .globl  _start
        .align  2
        .ent    _start
_start:
        lw      r5, r0, r1              // Get argc...
        addi    r6, r1, 4               // ... and argv ...
        addi    r7, r5, 1               // r7 = argc + 1
        add     r7, r7, r7              // ... r7 = (argc + 1) * 2 ...
        add     r7, r7, r7              // ... r7 = (argc + 1) * 4 ...
        add     r7, r7, r6              // ... and compute environ.
        brid    _estart                 // Let's go!
        nop
        .end    _start
        .size   _start, .-_start
