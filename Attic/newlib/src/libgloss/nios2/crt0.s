#
#   Copyright (C) 2003
#   by Jonah Graham (jgraham@altera.com).
#
#   This file is part of Newlib.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place - Suite 330,
#   Boston, MA 02111-1307, USA.  

	#
	# crt0.s
	#
	# NIOS2 startup code
	#

	# Startup code
	# ??? THIS IS NOT PROPER STARTUP CODE, IT
	# WON'T WORK ON THE PROCESSOR BECAUSE OF MISSING
	# CACHE INIT ROUTINES, PLEASE SEE ALTERA DOCS.
	# This will work with the simulator, which is
	# its real purpose.



	.section	.text
	.align	3
	.globl	_start
	.type	_start,@function

_start:

	# set up the stack pointer.
	# ??? we may want to check to see if
	# we should inherit caller's stack
	movhi	sp, %hiadj(nasys_stack_top)
	addi	sp, sp, %lo(nasys_stack_top)

	# make sure it is aligned
	nor	sp, sp, sp
	ori	sp, sp, 0x7 # clear lowest three bits
	nor	sp, sp, sp

	# set up the global pointer.
	movhi	gp, %hiadj(_gp)
	addi	gp, gp, %lo(_gp)

	#set up the exception temporary
	movhi	et, %hiadj(_end)
	addi	et, et, %lo(_end)
	
	movhi	r2, %hiadj(__start_2)
	addi	r2, r2, %lo(__start_2)
	jmp	r2

  
