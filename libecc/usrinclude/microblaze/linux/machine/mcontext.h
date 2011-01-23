/*	$NetBSD: mcontext.h,v 1.11 2009/12/14 00:46:04 matt Exp $	*/

/*-
 * Copyright (c) 1999, 2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Klaus Klein, and by Jason R. Thorpe of Wasabi Systems, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MICROBLAZE_MCONTEXT_H_
#define _MICROBLACE_MCONTEXT_H_

typedef	unsigned long	__greg_t;

typedef struct {
        __greg_t r1;
        __greg_t r2;
        __greg_t r3;
        __greg_t r4;
        __greg_t r5;
        __greg_t r6;
        __greg_t r7;
        __greg_t r8;
        __greg_t r9;
        __greg_t r10;
        __greg_t r11;
        __greg_t r12;
        __greg_t r13;
        __greg_t r14;
        __greg_t r15;
        __greg_t r16;
        __greg_t r17;
        __greg_t r18;
        __greg_t r19;
        __greg_t r20;
        __greg_t r21;
        __greg_t r22;
        __greg_t r23;
        __greg_t r24;
        __greg_t r25;
        __greg_t r26;
        __greg_t r27;
        __greg_t r28;
        __greg_t r29;
        __greg_t r30;
        __greg_t r31;
        __greg_t pc;
        __greg_t msr;
        __greg_t ear;
        __greg_t esr;
} __gregset_t;

typedef struct {
	__gregset_t	__gregs;
        unsigned long oldmask;
} mcontext_t;

#endif // _MICROBLAZE_MCONTEXT_H_
