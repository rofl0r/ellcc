.set noreorder

.global sigsetjmp
.type sigsetjmp,@function
sigsetjmp:
	lui $gp, %hi(_gp)       # RICH: was _gp_disp.
	addi $gp, %lo(_gp)      # RICH: was _gp_disp and addiu.
	beq $5, $0, 1f
	# addu $gp, $gp, $25 RICH: use when _gp_disp.
	subu $sp, $sp, 32
	sw $4, 20($sp)
	sw $ra, 24($sp)
	sw $gp, 28($sp)
	addu $6, $4, 124
	li $5, 0
	li $4, 1
	sw $4, -4($6)
	lw $25, %call16(sigprocmask)($gp)
	jalr $25
	nop
	lw $gp, 28($sp)
	lw $ra, 24($sp)
	lw $4, 20($sp)
	addu $sp, $sp, 32
1:	lw $25, %call16(setjmp)($gp)
	jr $25
	nop
