	.text

	.byte 0x0f, 0x1f, 0x0	
	.byte 0x0f, 0x1f, 0x40, 0x0	
	.byte 0x0f, 0x1f, 0x44, 0x0,  0x0	
	.byte 0x66, 0x0f, 0x1f, 0x44, 0x0,  0x0	
	.byte 0x0f, 0x1f, 0x80, 0x0,  0x0,  0x0, 0x0	
	.byte 0x0f, 0x1f, 0x84, 0x0,  0x0,  0x0, 0x0, 0x0
	.byte 0x66, 0x0f, 0x1f, 0x84, 0x0,  0x0, 0x0, 0x0, 0x0
	.byte 0x66, 0x2e, 0x0f, 0x1f, 0x84, 0x0, 0x0, 0x0, 0x0, 0x0

	nop (%eax) 
	nop %eax
	nop %ax
	nopl (%eax) 
	nopw (%eax) 
	nopl %eax
	nopw %ax

	.p2align 4