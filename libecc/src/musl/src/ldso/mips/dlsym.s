.set noreorder
.global dlsym
.type dlsym,@function
dlsym:
	lui $gp, %hi(_gp)       # RICH: was _gp_disp.
	addi $gp, %lo(_gp)      # RICH: was _gp_disp and addiu.
	# addu $gp, $gp, $25    RICH: use when _gp_disp
	move $6, $ra
	lw $25, %call16(__dlsym)($gp)
	addiu $sp, $sp, -16
	sw $ra, 12($sp)
	jalr $25
	nop
	lw $ra, 12($sp)
	jr $ra
	addiu $sp, $sp, 16
