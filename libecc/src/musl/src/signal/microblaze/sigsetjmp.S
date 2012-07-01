        .global sigsetjmp
        .type sigsetjmp,@function
sigsetjmp:
        lwi     r11, r5, 60
        beq     r11, r0, 1f
        addi    r7, r5, 64
        add     r6, r0, r0
        bralid  r15, sigprocmask
        ori     r5, r0, 2               // arg1 = 2 (delay)
1:      brai    setjmp
