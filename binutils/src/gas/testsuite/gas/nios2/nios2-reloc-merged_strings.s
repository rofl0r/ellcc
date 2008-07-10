	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"DHRYSTONE PROGRAM, 1'ST STRING"
.LC1:
	.string	"COMMON"
.LC2:
	.string	"DHRYSTONE PROGRAM, 2'ND STRING"
	
	 .section	.text
	 .align	3
	 .global	func1
	 .type	func1, @function
func1:
	#movhi	$2, %hiadj(.LC0)
	addi	r2, r2, .LC0
	#movhi	$3, %hiadj(.LC1)
	addi	r3, r3, .LC1
	#movhi	$4, %hiadj(.LC2)
	addi	r4, r4, .LC2
	.size	func1, .-func1

