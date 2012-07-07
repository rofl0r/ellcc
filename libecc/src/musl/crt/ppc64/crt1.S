#include <bits/asm.h>
        .weak  _init
        .weak  _fini
        .global _start
        .align  2
        .type   _start,@function
        .section ".opd", "aw",@progbits
_start:
        .align  3
        .quad   __start, .TOC.@tocbase
        .text
__start:
        mr      r9, r1                  // Save the original stack pointer.
        clrrdi  r1, r1, 4               // Align the stack to 16 bytes.
        li      r0, 0                   // Zero the frame pointer.
        stwu    r1, -128(r1)            // The initial stack frame.
        mtlr    r0                      // Clear the link register.
        std     r0, 0(r1)               // And save it.
        ld      r3, .Lmain@toc(r2)      // Get main() ...
        ld      r4, 0(r9)               // and argc...
        addi    r5, r9, 8               // and argv ...
        ld      r6, .L_init@toc(r2)     // and _init() ...
        ld      r7, .L_fini@toc(r2)     // and _fini() ...
        li      r8, 0                   // ldso_fini == NULL
        bl       __libc_start_main      // Let's go!
        b       .                       // Never gets here.
        .size   __start, .-__start

       .section        .toc,"aw",@progbits
.Lmain:
        .tc main[TC],main
.L_init:
        .tc _init[TC],_init
.L_fini:
        .tc _fini[TC],_fini
