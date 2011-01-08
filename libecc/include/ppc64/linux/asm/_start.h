/** The initial entry point.
 */
asm(
"       .section \".opd\",\"aw\"\n"
"       .align  3\n"
"_start:\n"
"       .quad   ._start, .TOC.@tocbase, 0\n"
"       .size _start, 24\n"
"       .section \".text\"\n"
"       .globl  ._start\n"
"       .type   ._start, %function\n"
"       .align  3\n"
"._start:\n"
"       mr      9, 1\n"                 // Save the original stack pointer.
"       clrrdi  1, 1, 4\n"              // Align the stack  to 16 bytes. */
"       li      0, 0\n"                 // Clear the frame pointer.
"       stdu    1, -128(1)\n"           // The initial stack frame. RICH?
"       mtlr    0\n"                    // Clear the link register.
"       std     0, 0(1)\n"              // And save it.
"       lwz     3, 0(9)\n"              // Get argc...
"       addi    4, 9, 8\n"              // ... and argv ...
"       addi    5, 3, 1\n"              // ... and argc + 1 ...
"       add     5, 5, 5\n"              // ... (argc + 1) * 2
"       add     5, 5, 5\n"              // ... (argc + 1) * 4
"       add     5, 5, 5\n"              // ... (argc + 1) * 8 to compute environ.
"       add     5, 5, 4\n"              // ... indexed from argv.
"       b       _estart\n"              // Let's go!
"       nop\n"
"       .size   ._start, .-._start\n"
);

