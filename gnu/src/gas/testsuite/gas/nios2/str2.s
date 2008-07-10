	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"DHRYSTONE PROGRAM, 3'RD STRING"
.LC1:
	.string	"COMMON"
.LC2:
	.string	"DHRYSTONE PROGRAM, 4'TH STRING"
	
	 .section	.text
	 .align	3
	 .global	func2
	 .type	func2, @function
func2:
	#movhi	r2, %hiadj(.LC0)
	addi	r2, r2, .LC0
	#movhi	r3, %hiadj(.LC1)
	addi	r3, r3, .LC1
	#movhi	r4, %hiadj(.LC2)
	addi	r4, r4, .LC2
	.size	func2, .-func2

