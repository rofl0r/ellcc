	.file	"sieve.ubc"


	.text
	.align	16
	.globl	main
	.type	main,@function
main:
	subl	$44, %esp
	movl	48(%esp), %eax
	movl	52(%esp), %ecx
	movl	%eax, 40(%esp)
	movl	%ecx, 36(%esp)
	movl	$0, 32(%esp)
	movl	$.str, (%esp)
	call	printf
	movl	$1, 12(%esp)
.LBB1_1:
	movl	12(%esp), %eax
	cmpl	$10000, %eax
	jg	.LBB1_4
	movl	$0, 16(%esp)
	movl	$0, 28(%esp)
	jmp	.LBB1_5
.LBB1_3:
	movl	12(%esp), %eax
	movl	12(%esp), %eax
	addl	$1, %eax
	movl	%eax, 12(%esp)
	jmp	.LBB1_1
.LBB1_4:
	movl	16(%esp), %eax
	movl	$.str1, (%esp)
	movl	%eax, 4(%esp)
	call	printf
	jmp	.LBB1_19
.LBB1_5:
	movl	28(%esp), %eax
	cmpl	$8190, %eax
	jg	.LBB1_8
	movl	28(%esp), %eax
	movb	$1, flags(,%eax)
	movl	28(%esp), %eax
	movl	28(%esp), %eax
	addl	$1, %eax
	movl	%eax, 28(%esp)
	jmp	.LBB1_5
.LBB1_8:
	movl	$0, 28(%esp)
.LBB1_9:
	movl	28(%esp), %eax
	cmpl	$8190, %eax
	jg	.LBB1_12
	movl	28(%esp), %eax
	movb	flags(,%eax), %al
	cmpb	$0, %al
	jne	.LBB1_13
	jmp	.LBB1_14
.LBB1_11:
	movl	28(%esp), %eax
	movl	28(%esp), %eax
	addl	$1, %eax
	movl	%eax, 28(%esp)
	jmp	.LBB1_9
.LBB1_12:
	jmp	.LBB1_3
.LBB1_13:
	movl	28(%esp), %eax
	movl	28(%esp), %ecx
	addl	%eax, %ecx
	addl	$3, %ecx
	movl	%ecx, 24(%esp)
	movl	28(%esp), %eax
	movl	24(%esp), %ecx
	addl	%eax, %ecx
	movl	%ecx, 20(%esp)
	jmp	.LBB1_16
.LBB1_14:
.LBB1_15:
	jmp	.LBB1_11
.LBB1_16:
	movl	20(%esp), %eax
	cmpl	$8190, %eax
	jg	.LBB1_18
	movl	20(%esp), %eax
	movb	$0, flags(,%eax)
	movl	20(%esp), %eax
	movl	24(%esp), %ecx
	addl	%eax, %ecx
	movl	%ecx, 20(%esp)
	jmp	.LBB1_16
.LBB1_18:
	movl	16(%esp), %eax
	addl	$1, %eax
	movl	%eax, 16(%esp)
	jmp	.LBB1_15
.LBB1_19:
	movl	32(%esp), %eax
	addl	$44, %esp
	ret
	.size	main, .-main
