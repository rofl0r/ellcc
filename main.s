	.file	"main.ubc"
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1


	.text
	.globl	main
	.align	2
main:
	sub sp, sp, #8
	sub sp, sp, #12
	str r3, [sp, #+16]
	str r2, [sp, #+12]
	str r0, [sp, #+8]
	str r1, [sp, #+4]
	mov r0, #0
	str r0, [sp]
.LBB1_1:
	ldr r0, [sp]
	add sp, sp, #12
	add sp, sp, #8
	bx lr
	.size main, .-main

