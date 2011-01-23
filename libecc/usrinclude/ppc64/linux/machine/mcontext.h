/*	$NetBSD: mcontext.h,v 1.8 2008/04/28 20:23:32 martin Exp $	*/

/*-
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Klaus Klein.
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

#ifndef _POWERPC_MCONTEXT_H_
#define _POWERPC_MCONTEXT_H_

#define	_NGREG	48
#define _NFPREG 33
#define _NVRREG 34

typedef	unsigned long	__greg_t;
typedef	__greg_t	__gregset_t[_NGREG];
typedef double          __fpregset_t[_NFPREG];

typedef struct vscr {
        unsigned        pad[3];
        unsigned        vscr_word;
} __vscr_t;

typedef struct vrstate {
        unsigned        vrregs[32][4];
        __vscr_t        vscr;
        unsigned        vrsave;
        unsigned        pad[3];
} __vrregset_t  __attribute__((__aligned__(16)));

typedef struct {
        unsigned        unused[4];
        int             signal;
        int             pad0;
        unsigned long   handler;
        unsigned long   oldmask;
        struct pt_regs  *regs;
        __gregset_t     gp_regs;
        __fpregset_t    fp_regs;
        __vrregset_t    *v_regs;
        long            vmx_reserve[_NVRREG + _NVRREG + 1];
} mcontext_t;

#endif	/* !_POWERPC_MCONTEXT_H_ */
