/** The initial entry point.
 */
asm(
"       .text\n"
"       .align  16\n"
"       .globl  _start\n"
"       .type   _start, #function\n"
"_start:\n"
"       clr     %fp\n"                  // Clear the link register.
"       ld      [%sp], %o0\n"           // Get argc...
"       add     %sp, 4, %o1\n"          // ... and argv ...
"       add     %o0, %o0, %o2\n"
"       add     %o2, %o2, %o2\n"
"       add     %o1, %o2, %o2\n"        // ... and compute environ.
"       call    _estart\n"              // Let's go!
"       nop\n"
"       b       .\n"                    // Never gets here.
"       nop\n"
);

