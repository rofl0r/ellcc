/** The initial entry point.
 */
asm(
"       .text\n"
"       .globl  _start\n"
"       .type   _start, %function\n"
"_start:\n"
"       mr      9, 1\n"                 // Save the original stack pointer.
"       clrrwi  1, 1, 4\n"              // Align the stack  to 16 bytes.
"       li      0, 0\n"                 // Clear the frame pointer.
"       stwu    1, -16(1)\n"            // The initial stack frame.
"       mtlr    0\n"                    // Clear the link register.
"       stw     0, 0(1)\n"              // And save it.
"       lwz     4, 0(9)\n"              // Get argc...
"       addi    5, 9, 4\n"              // ... and argv ...
"       addi    6, 4, 1\n"              // ... and argc + 1 ...
"       add     6, 6, 6\n"              // ... (argc + 1) * 2
"       add     6, 6, 6\n"              // ... (argc + 1) * 4 to compute environ.
"       b       _estart\n"              // Let's go!
"       .size   _start, .-_start\n"
);

