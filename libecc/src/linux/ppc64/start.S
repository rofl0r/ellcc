/** The initial entry point.
 */
        .section ".opd", "aw"
        .align  3
        .globl  _start
_start:
        .quad   __start, .TOC.@tocbase, 0
        .size   _start, 24
        .section ".text"
        .type   __start, %function
        .align  3
__start:
        mr      9, 1                    // Save the original stack pointer.
        clrrdi  1, 1, 4                 // Align the stack  to 16 bytes. */
        li      0, 0                    // Clear the frame pointer.
        stdu    1, -128(1)              // The initial stack frame.
        mtlr    0                       // Clear the link register.
        std     0, 0(1)                 // And save it.
        ld      3, 0(9)                 // Get argc...
        addi    4, 9, 8                 // ... and argv ...
        addi    5, 3, 1                 // ... and argc + 1 ...
        add     5, 5, 5                 // ... (argc + 1) * 2
        add     5, 5, 5                 // ... (argc + 1) * 4
        add     5, 5, 5                 // ... (argc + 1) * 8 to compute environ.
        add     5, 5, 4                 // ... indexed from argv.
        b       _estart                 // Let's go!
        nop
        .size   __start, .-__start
