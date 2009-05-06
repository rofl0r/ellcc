;/*
; * C library -- _setjmp, _longjmp
; *
; *      _longjmp(a,v)
; * will generate a "return(v?v:1)" from
; * the last call to
; *      _setjmp(a)
; * by unwinding the call stack.
; * The previous signal state is NOT restored.
; */
;
;
	.section	.text
	.align	3
	.globl	setjmp
	.type	setjmp,@function
	.globl	longjmp
	.type	longjmp,@function


setjmp:
	stw	r16, 0(r4)
	stw	r17, 4(r4)
	stw	r18, 8(r4)
	stw	r19, 12(r4)
	stw	r20, 16(r4)
	stw	r21, 20(r4)
	stw	r22, 24(r4)
	stw	r23, 28(r4)
	stw	gp, 32(r4)
	stw	sp, 36(r4)
	stw	fp, 40(r4)
	stw	ra, 44(r4)
	mov	r2, zero
	ret

longjmp:
	ldw	r16, 0(r4)
	ldw	r17, 4(r4)
	ldw	r18, 8(r4)
	ldw	r19, 12(r4)
	ldw	r20, 16(r4)
	ldw	r21, 20(r4)
	ldw	r22, 24(r4)
	ldw	r23, 28(r4)
	ldw	gp, 32(r4)
	ldw	sp, 36(r4)
	ldw	fp, 40(r4)
	ldw	ra, 44(r4)
	mov	r2, r5
	bne	r2, zero, 1f
	movi	r2, 1	
1:
	ret
