#include <bits/asm.h>
        .text
        .global dlsym
        .align  2
        .type   dlsym,@function
        .ent    dlsym
dlsym:
        move    a2, ra                  // The return address is arg3.
        lw      t0, %call16(__dlsym)(gp)
        jr      t0
        .end    dlsym
        .size   dlsym, .-dlsym
