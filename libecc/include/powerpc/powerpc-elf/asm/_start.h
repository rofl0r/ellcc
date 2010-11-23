/** The initial entry point.
 */
asm(
"       .text\n"
"       .global _start\n"
"       .type   _start, @function\n"
"       .align  2\n"
"_start:\n"
"       li      0, 0\n"
"       mtlr    0\n"                    // Clear the link register.
"       bl      _estart\n"              // Let's go!
"       b       .\n"                    // Never gets here.
"       .size   _start,.-_start\n"
);

