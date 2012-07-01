        .weak  _init
        .weak  _fini
        .global _start
        .align  2
        .ent    _start
_start:
        .frame  r19, 4*6, r15
        add     r19, r0, r0             // Zero the frame pointer.
        ori     r5, r0, main            // Get main() ...
        lw      r6, r0, r1              // Get argc...
        addi    r7, r1, 4               // ... and argv ...
        ori     r8, r0, _init           // and _init() ...
        ori     r9, r0, _fini           // and _fini() ...
        add     r10, r0, r0             // ldso_fini == NULL
        addik   r1, r1, -4*6            // LEave space for the args.
        bri     main                    // Let's go!
                                        // Never gets here.
        .end    _start
        .size   _start, .-_start
