	.file	"_exit.ubc"
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1


	.text
	.globl	_exit
	.align	2
_exit:
	stmfd sp!, {r4, lr}
	mov r4, r0
.LBB1_1:
	mov r1, r4
	@ InlineAsm Start
	swi	#231	@ syscall SYS_ify(exit_group)
	@ InlineAsm End
	ldr r0, .LCPI1_0
	cmp r1, r0
	blo .LBB1_3	@ next2
.LBB1_2:
	rsb r0, r1, #0
	bl __set_errno
.LBB1_3:
	mov r1, r4
	@ InlineAsm Start
	swi	#60	@ syscall SYS_ify(exit)
	@ InlineAsm End
	ldr r0, .LCPI1_0
	cmp r1, r0
	blo .LBB1_1	@ body
.LBB1_4:
	rsb r0, r1, #0
	bl __set_errno
	b .LBB1_1	@ body
.LBB1_5:
	.LCPI1_0:
 	.long	4294963201

	.size _exit, .-_exit

