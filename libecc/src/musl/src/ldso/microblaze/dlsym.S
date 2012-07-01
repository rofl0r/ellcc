        .text
        .global dlsym
        .align  2
        .type   dlsym,@function
        .ent    dlsym
dlsym:
        add     r7, r15, r0             // The return address is arg3.
        brid    __dlsym
        .end    dlsym
        .size   dlsym, .-dlsym
