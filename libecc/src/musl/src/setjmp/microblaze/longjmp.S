        .global _longjmp
        .global longjmp
        .type   _longjmp,@function
        .type   longjmp,@function
        .align  2
_longjmp:
longjmp:
        addi    r3, r6, 0
        bne     r3, r0, 1f
        addi    r3, r3, 1
1:      lwi     r1,  r5, 0
        lwi     r15, r5, 4
        lwi     r18,  r5, 8
        lwi     r19, r5, 12
        lwi     r20, r5, 16
        lwi     r21, r5, 20
        lwi     r22, r5, 24
        lwi     r23, r5, 28
        lwi     r24, r5, 32
        lwi     r25, r5, 36
        lwi     r26, r5, 40
        lwi     r27, r5, 44
        lwi     r28, r5, 48
        lwi     r29, r5, 52
        lwi     r30, r5, 56
	rtsd    r15, 8
        nop
