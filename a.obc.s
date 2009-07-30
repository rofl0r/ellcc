	.file	"a.obc"


	.text
	.align	16
	.globl	main
	.type	main,@function
main:
.LBB1_0:	# entry
	pushl	%ebx
	pushl	%edi
	pushl	%esi
	subl	$8, %esp
	movl	$str, (%esp)
	call	puts
	movl	$10000, %eax
.LBB1_1:	# continue3.outer
	xorl	%ecx, %ecx
	.align	16
.LBB1_2:	# continue3
	movb	$1, flags(%ecx)
	incl	%ecx
	cmpl	$8191, %ecx
	jne	.LBB1_2	# continue3
.LBB1_3:	# continue3.body6_crit_edge
	xorl	%ecx, %ecx
	movl	$3, %edx
	movl	%edx, %esi
	movl	%ecx, %edi
.LBB1_4:	# body6
	cmpb	$0, flags(%ecx)
	jne	.LBB1_8	# ifTrue
.LBB1_5:	# continue7
	addl	$3, %edx
	addl	$2, %esi
	incl	%ecx
	cmpl	$8191, %ecx
	jne	.LBB1_4	# body6
.LBB1_6:	# test5.continue_crit_edge
	decl	%eax
	jne	.LBB1_1	# continue3.outer
.LBB1_7:	# return
	movl	%edi, 4(%esp)
	movl	$.str1, (%esp)
	call	printf
	xorl	%eax, %eax
	addl	$8, %esp
	popl	%esi
	popl	%edi
	popl	%ebx
	ret
.LBB1_8:	# ifTrue
	cmpl	$8190, %edx
	jg	.LBB1_11	# next12
.LBB1_9:	# ifTrue.body11_crit_edge
	movl	%edx, %ebx
	.align	16
.LBB1_10:	# body11
	movb	$0, flags(%ebx)
	addl	%esi, %ebx
	cmpl	$8191, %ebx
	jl	.LBB1_10	# body11
.LBB1_11:	# next12
	incl	%edi
	jmp	.LBB1_5	# continue7
	.size	main, .-main
	.type	flags,@object
	.bss
	.local	flags
	.comm	flags,8191,16		# flags
	.type	.str1,@object
	.section	.rodata.str1.1,"aMS",@progbits,1
.str1:				# .str1
	.size	.str1, 11
	.asciz	"%d primes\n"
	.type	str,@object
	.align	16
str:				# str
	.size	str, 17
	.asciz	"10000 iterations"

	.section	.note.GNU-stack,"",@progbits
