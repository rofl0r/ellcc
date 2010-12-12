/** The initial entry point.
 */
asm(
"       .text\n"
"       .globl  _start\n"
"       .align  2\n"
"       .ent    _start\n"
"_start:\n"
"       lw      r5, r0, r1\n"           // Get argc...
"       addi    r6, r1, 4\n"            // ... and argv ...
"       addi    r7, r5, 1\n"            // r7 = argc + 1
"       add     r7, r7, r7\n"           // ... r7 = (argc + 1) * 2 ...
"       add     r7, r7, r7\n"           // ... r7 = (argc + 1) * 4 ...
"       add     r7, r7, r6\n"           // ... and compute environ.
"       la      r2, r0, _SDA_BASE_\n"   // Get the SDAs.
"       la      r13, r0, _SDA2_BASE_\n"
"       brid    _estart\n"              // Let's go!
"       nop\n"
"       .end    _start\n"
"       .size   _start, .-_start\n"
);

