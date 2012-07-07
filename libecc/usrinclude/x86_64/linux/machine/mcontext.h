/*	$NetBSD: mcontext.h,v 1.7 2008/04/28 20:23:24 martin Exp $	*/

/*-
 * Copyright (c) 1999 The NetBSD Foundation, Inc.
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

#ifndef _X86_64_MCONTEXT_H_
#define _X86_64_MCONTEXT_H_


/*
 * mcontext extensions to handle signal delivery.
 */
#define _UC_SETSTACK	0x00010000
#define _UC_CLRSTACK	0x00020000
#define _UC_VM		0x00040000

/*
 * Layout of mcontext_t according to the System V Application Binary Interface,
 * Intel386(tm) Architecture Processor Supplement, Fourth Edition.
 */  

/*
 * General register state
 */
#define _NGREG		23
typedef	long		__greg_t;
typedef	__greg_t	__gregset_t[_NGREG];

#define _REG_R8         0
#define _REG_R9         1
#define _REG_R10        2
#define _REG_R11        3
#define _REG_R12        4
#define _REG_R13        5
#define _REG_R14        6
#define _REG_R15        7
#define _REG_RDI	8
#define _REG_RSI	9
#define _REG_RBP	10
#define _REG_EBX	11
#define _REG_EDX	12
#define _REG_EAX	13
#define _REG_ECX	14
#define _REG_RSP	15
#define _REG_RIP	16
#define _REG_EFL	17
#define _REGCSGSFS      18      /* short cs, gs, fs, 0 */
#define _REG_ERR	19
#define _REG_TRAPNO	20
#define _REG_OLDMASK	21
#define _REG_CR2	22

struct fpreg {
        unsigned short  significand[4];
        unsigned short  exponent;
        unsigned short  pad[3];
};

struct xmmreg {
        uint32_t        element[4];
};

struct fpstate {
        uint16_t        cwd;
        uint16_t        swd;
        uint16_t        ftw;
        uint16_t        fop;
        uint64_t        rip;
        uint64_t        rdp;
        uint32_t        mxcsr;
        struct fpreg    _st[8];
        struct xmmreg   _xmm[16];
        uint32_t        pad[24];
};

typedef struct fpstate  *fpregset_t;

typedef struct {
        __gregset_t     gregs;
        fpregset_t      fpregs;
        unsigned long   cr2;
        unsigned long   reserved[8];
} mcontext_t;

#endif // _X86_64_MCONTEXT_H_
