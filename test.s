	.file	"test.ubc"


	.text
	.align	16
	.globl	f
	.type	f,@function
f:
	subl	$12, %esp
	movl	$0, 8(%esp)
	movl	$0, 4(%esp)
	movl	4(%esp), %eax
	movl	%eax, (%esp)
	movl	8(%esp), %eax
	addl	$12, %esp
	ret
	.size	f, .-f
	.type	x,@object
	.bss
	.globl x
	.align	4
x:
	.size	x, 4
	.zero	4
	.type	k,@object
	.data
	.globl k
	.align	4
k:
	.size	k, 4
	.long	2

	.section	.note.GNU-stack,"",@progbits
