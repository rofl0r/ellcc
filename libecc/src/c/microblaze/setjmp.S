/*	$NetBSD: setjmp.S,v 1.1 2001/06/19 00:25:02 fvdl Exp $	*/

/*
 * Copyright (c) 2001 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Frank van der Linden for Wasabi Systems, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed for the NetBSD Project by
 *      Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#include <machine/asm.h>
#include <machine/setjmp.h>

#if defined(LIBC_SCCS)
	RCSID("$NetBSD: __setjmp14.S,v 1.1 2001/06/19 00:25:02 fvdl Exp $")
#endif

/*
 * C library -- setjmp, longjmp
 *
 *	longjmp(a,v)
 * will generate a "return(v)" from the last call to
 *	setjmp(a)
 * by restoring registers from the stack.
 * The previous signal state is restored.
 */

ENTRY(setjmp)
        // Save registers relative to r5 (env)
        swi     r1, r5, (_JB_REG_R1 * 4)        // sp
        swi     r15, r5, (_JB_REG_R15 * 4)      // lr
        swi     r18, r5, (_JB_REG_R18 * 4)
        swi     r19, r5, (_JB_REG_R19 * 4)
        swi     r20, r5, (_JB_REG_R20 * 4)
        swi     r21, r5, (_JB_REG_R21 * 4)
        swi     r22, r5, (_JB_REG_R22 * 4)
        swi     r23, r5, (_JB_REG_R23 * 4)
        swi     r24, r5, (_JB_REG_R24 * 4)
        swi     r25, r5, (_JB_REG_R25 * 4)
        swi     r26, r5, (_JB_REG_R26 * 4)
        swi     r27, r5, (_JB_REG_R27 * 4)
        swi     r28, r5, (_JB_REG_R28 * 4)
        swi     r29, r5, (_JB_REG_R29 * 4)
        swi     r30, r5, (_JB_REG_R30 * 4)

	addi	r7, r5, (_JB_SIGMASK * 8)       // Address of the mask.
        addi    r5, r0, 0                       // how (ignored).
	brid	_C_LABEL(sigprocmask)           // Tail call.
        addi    r6, r0, 0                       // set (NULL). (delay)
END(setjmp)

ENTRY(longjmp)
        addi     r19. r5, 0                     // Save the env pointer.
	addi	r6, r5, (_JB_SIGMASK * 8)       // Address of the mask.
        addi    r5, r0, 3                       // how (SIG_SETMASK).
	brlid	r15, _C_LABEL(sigprocmask)      // Call. No need to preserve lr.
        addi    r7, r0, 0                       // oldset (NULL). (delay)

        lwi     r1, r19, (_JB_REG_R1 * 4)       // sp
        lwi     r15, r19, (_JB_REG_R15 * 4)     // lr
        lwi     r18, r19, (_JB_REG_R18 * 4)
        lwi     r19, r19, (_JB_REG_R19 * 4)
        lwi     r20, r19, (_JB_REG_R20 * 4)
        lwi     r21, r19, (_JB_REG_R21 * 4)
        lwi     r22, r19, (_JB_REG_R22 * 4)
        lwi     r23, r19, (_JB_REG_R23 * 4)
        lwi     r24, r19, (_JB_REG_R24 * 4)
        lwi     r25, r19, (_JB_REG_R25 * 4)
        lwi     r26, r19, (_JB_REG_R26 * 4)
        lwi     r27, r19, (_JB_REG_R27 * 4)
        lwi     r28, r19, (_JB_REG_R28 * 4)
        lwi     r29, r19, (_JB_REG_R29 * 4)
        lwi     r30, r19, (_JB_REG_R30 * 4)
	rtid    r15, 8
        addi    r3, r0, 1                       // return 1 (delay)
END(longjmp)
