        .global _start
        .type   _start,@function
        .align  2
        .ent    _start
_start:
        add     r19, r0, r0             // Zero the frame pointer.
        lw      r6, r0, r1              // Get argc...
        addi    r7, r1, 4               // ... and argv ...
        // Get the musl dynamic linker entry point.
        brlid   r15, __dynlink
        nop
        ori     r11, r0, 1
        beq     r3, r11, .               // Stay here.
        add     r5, r0, r0
        bra     r3                       // Otherwise go.
        nop
        .end    _start
        .size   _start, .-_start

