/** The initial entry point.
 */
        .text
        .globl  _start
        .align  2
_start:
        sub     lr, lr, lr              // Clear the link register.
        ldr     r0, [sp]                // Get argc...
        add     r1, sp, #4              // ... and argv ...
        add     r2, r0, #1              // ... and argc + 1 ...
        add     r2, r1, r2, LSL #2      // ... and compute environ.
        bl      _estart                 // Let's go!
        b       .                       // Never gets here.
        .size   _start, .-_start
