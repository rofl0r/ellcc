	beq r2, r3, out_of_range
	bne r2, r3, in_range
	nop
	nop
in_range:
	nop
.align 15
	br 0
out_of_range:
	nop
