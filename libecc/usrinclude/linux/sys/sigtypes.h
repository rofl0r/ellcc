/*	$NetBSD: sigtypes.h,v 1.9 2008/10/15 06:51:21 wrstuden Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1989, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)signal.h	8.4 (Berkeley) 5/4/95
 */

/* rdp - This file has been updated to contain the Linux definitions. */

#ifndef	_SYS_SIGTYPES_H_
#define	_SYS_SIGTYPES_H_

/*
 * This header file defines various signal-related types.  We also keep
 * the macros to manipulate sigset_t here, to encapsulate knowledge of
 * its internals.
 */

#include <sys/featuretest.h>
#include <machine/int_types.h>
#include <machine/ansi.h>

#ifdef	_BSD_SIZE_T_
typedef	_BSD_SIZE_T_	size_t;
#undef	_BSD_SIZE_T_
#endif

#if defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE) || \
    defined(_NETBSD_SOURCE)

/* There are 1024 bits in a sigset_t. */
#define _SIGSET_NWORDS (1024 / (8 * sizeof(unsigned long)))
typedef struct {
	unsigned long __bits[_SIGSET_NWORDS];
} sigset_t;

/*
 * Macro for manipulating signal masks.
 */
#define __sigmask(n)		(1UL << (((n) - 1) % (8 * sizeof(unsigned long))))
#define	__sigword(n)		(((unsigned int)(n) - 1) / (8 * sizeof(unsigned long)))
#define	__sigaddset(s, n)	((s)->__bits[__sigword(n)] |= __sigmask(n))
#define	__sigdelset(s, n)	((s)->__bits[__sigword(n)] &= ~__sigmask(n))
#define	__sigismember(s, n)	(((s)->__bits[__sigword(n)] & __sigmask(n)) != 0)

#define	__sigemptyset(s)	                                        \
 (__extension__ ({ int count;                                           \
                   for (count = 0; count < _SIGSET_NWORDS; ++ count) {  \
                       (s)->__bits[count] = 0;                          \
                   }                                                    \
                   0; }))

#define	__sigfillset(s)	                                                \
 (__extension__ ({ int count;                                           \
                   for (count = 0; count < _SIGSET_NWORDS; ++ count) {  \
                       (s)->__bits[count] = ~0UL;                       \
                   }                                                    \
                   0; }))

#define	__sigsetequal(s1, s2)	                                        \
 (__extension__ ({ int count;                                           \
                   int result = 1;                                      \
                   for (count = 0; count < _SIGSET_NWORDS; ++ count) {  \
                       if ((s1)->__bits[count] != (s1)->__bits[count]) {\
                           result = 0;                                  \
                           break;                                       \
                       }                                                \
                   }                                                    \
                   result; }))

#define	__sigplusset(s, t)	                                        \
 (__extension__ ({ int count;                                           \
                   for (count = 0; count < _SIGSET_NWORDS; ++ count) {  \
                       (t)->__bits[count] |= (s)->__bits[count]);       \
                   }                                                    \
                   0; }))

#define	__sigminusset(s, t)	                                        \
 (__extension__ ({ int count;                                           \
                   for (count = 0; count < _SIGSET_NWORDS; ++ count) {  \
                       (t)->__bits[count] &= ~(s)->__bits[count]);      \
                   }                                                    \
                   0; }))

#define	__sigandset(s, t)	                                        \
 (__extension__ ({ int count;                                           \
                   for (count = 0; count < _SIGSET_NWORDS; ++ count) {  \
                       (t)->__bits[count] &= (s)->__bits[count]);       \
                   }                                                    \
                   0; }))

#if (defined(_XOPEN_SOURCE) && defined(_XOPEN_SOURCE_EXTENDED)) || \
    (_XOPEN_SOURCE - 0) >= 500 || defined(_NETBSD_SOURCE)
typedef struct
#if defined(_NETBSD_SOURCE)
               sigaltstack
#endif /* _NETBSD_SOURCE */
			   {
	void	*ss_sp;			/* signal stack base */
	int	ss_flags;		/* SS_DISABLE and/or SS_ONSTACK */
	size_t	ss_size;		/* signal stack length */
} stack_t;

#endif /* _XOPEN_SOURCE_EXTENDED || XOPEN_SOURCE >= 500 || _NETBSD_SOURCE */

#endif	/* _POSIX_C_SOURCE || _XOPEN_SOURCE || ... */

#if defined(_NETBSD_SOURCE)
/*
 * Now include types used by Scheduler Activations. Included here as
 * this is the best standard header to add them to.
 */
#include <sys/satypes.h>
#endif

#endif	/* !_SYS_SIGTYPES_H_ */
