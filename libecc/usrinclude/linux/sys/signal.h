/*	$NetBSD: signal.h,v 1.65 2009/12/09 20:26:12 drochner Exp $	*/

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

#ifndef	_SYS_SIGNAL_H_
#define	_SYS_SIGNAL_H_

#include <sys/featuretest.h>
#include <sys/sigtypes.h>

#define _NSIG		32

#if defined(_NETBSD_SOURCE)
#define NSIG _NSIG

#endif /* _NETBSD_SOURCE */

#define	SIGHUP		1	/* hangup */
#define	SIGINT		2	/* interrupt */
#define	SIGQUIT		3	/* quit */
#define	SIGILL		4	/* illegal instruction (not reset when caught) */
#define	SIGTRAP		5	/* trace trap (not reset when caught) */
#define	SIGABRT		6	/* abort() */
#define	SIGIOT		SIGABRT	/* compatibility */
#define	SIGBUS		7	/* bus error */
#define	SIGFPE		8	/* floating point exception */
#define	SIGKILL		9	/* kill (cannot be caught or ignored) */
#define	SIGUSR1		10	/* user defined signal 1 */
#define SIGINFO         SIGUSR1 /* For NetBSD compatability */
#define	SIGSEGV		11	/* segmentation violation */
#define	SIGUSR2		12	/* user defined signal 2 */
#define	SIGPIPE		13	/* write on a pipe with no one to read it */
#define	SIGALRM		14	/* alarm clock */
#define	SIGTERM		15	/* software termination signal from kill */
#define SIGSTKFLT       16      /* stack fault */
#define	SIGCHLD		17	/* to parent on child stop or exit */
#define	SIGCLD		SIGCHLD	/* compatibility */
#define	SIGCONT		18	/* continue a stopped process */
#define	SIGSTOP		19	/* sendable stop signal not from tty */
#define	SIGTSTP		20	/* stop signal from tty */
#define	SIGTTIN		21	/* to readers pgrp upon background tty read */
#define	SIGTTOU		22	/* like TTIN for output if (tp->t_local&LTOSTOP) */
#define	SIGURG		23	/* urgent condition on IO channel */
#define	SIGXCPU		24	/* exceeded CPU time limit */
#define	SIGXFSZ		25	/* exceeded file size limit */
#define	SIGVTALRM	26	/* virtual time alarm */
#define	SIGPROF		27	/* profiling time alarm */
#define	SIGWINCH	28	/* window size changes */
#define	SIGIO		29	/* input/output possible signal */
#define SIGPOLL         SIGIO   /* pollable event occured */
#define	SIGPWR		30	/* power fail/restart (not reset when caught) */
#define	SIGSYS		31	/* bad argument to system call */

#define	SIGRTMIN	32
#define	SIGRTMAX	_NSIG

#ifndef _KERNEL
#include <sys/cdefs.h>
#endif

#define	SIG_DFL		((void (*)(int))  0)
#define	SIG_IGN		((void (*)(int))  1)
#define	SIG_ERR		((void (*)(int)) -1)
#define	SIG_HOLD	((void (*)(int))  2)

#if defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE) || \
    defined(_NETBSD_SOURCE)

#ifdef _KERNEL
#define	sigaddset(s, n)		__sigaddset(s, n)
#define	sigdelset(s, n)		__sigdelset(s, n)
#define	sigismember(s, n)	__sigismember(s, n)
#define	sigemptyset(s)		__sigemptyset(s)
#define	sigfillset(s)		__sigfillset(s)
#define sigplusset(s, t)	__sigplusset(s, t)
#define sigminusset(s, t)	__sigminusset(s, t)
#endif /* _KERNEL */

#if (_POSIX_C_SOURCE - 0) >= 199309L || (_XOPEN_SOURCE - 0) >= 500 || \
    defined(_NETBSD_SOURCE)
#include <sys/siginfo.h>
#endif

#if (defined(_XOPEN_SOURCE) && defined(_XOPEN_SOURCE_EXTENDED)) || \
    (_XOPEN_SOURCE - 0) >= 500 || defined(_NETBSD_SOURCE)
#include <sys/ucontext.h>
#endif /* _XOPEN_SOURCE_EXTENDED || _XOPEN_SOURCE >= 500 || _NETBSD_SOURCE */

/*
 * Signal vector "template" used in sigaction call.
 */
struct	sigaction {
	union {
		void (*_sa_handler)(int);
#if (_POSIX_C_SOURCE - 0) >= 199309L || (_XOPEN_SOURCE - 0) >= 500 || \
    defined(_NETBSD_SOURCE)
		void (*_sa_sigaction)(int, siginfo_t *, void *);
#endif
	} _sa_u;	/* signal handler */
	sigset_t sa_mask;		/* signal mask to apply */
	int	sa_flags;		/* see signal options below */
        void (*sa_restorer)(void);      /* restore handler */
};

#define sa_handler _sa_u._sa_handler
#if (_POSIX_C_SOURCE - 0) >= 199309L || (_XOPEN_SOURCE - 0) >= 500 || \
    defined(_NETBSD_SOURCE)
#define sa_sigaction _sa_u._sa_sigaction
#endif

#include <machine/signal.h>	/* sigcontext; codes for SIGILL, SIGFPE */

/* Only valid for SIGCHLD. */
#define SA_NOCLDSTOP	0x0000000001	/* do not generate SIGCHLD on child stop */
#define SA_NOCLDWAIT	0x0000000002	/* do not generate zombies on unwaited child */
#if (_POSIX_C_SOURCE - 0) >= 199309L || (_XOPEN_SOURCE - 0) >= 500 || \
    defined(_NETBSD_SOURCE)
#define SA_SIGINFO	0x0000000004	/* take sa_sigaction handler */
#endif /* (_POSIX_C_SOURCE - 0) >= 199309L || ... */
#if (defined(_XOPEN_SOURCE) && defined(_XOPEN_SOURCE_EXTENDED)) || \
    (_XOPEN_SOURCE - 0) >= 500 || defined(_NETBSD_SOURCE)
#define SA_ONSTACK	0x08000000	/* take signal on signal stack */
#define SA_RESTART	0x10000000	/* restart system call on signal return */
#define SA_NODEFER	0x40000000	/* don't mask the signal we're delivering */
#define SA_RESETHAND	0x80000000	/* reset to SIG_DFL when taking signal */
#endif /* _XOPEN_SOURCE_EXTENDED || XOPEN_SOURCE >= 500 || _NETBSD_SOURCE */
#ifdef _KERNEL
#define	SA_ALLBITS	0xFFFFFFFF
#endif

/*
 * Flags for sigprocmask():
 */
#define	SIG_BLOCK	0	/* block specified signal set */
#define	SIG_UNBLOCK	1	/* unblock specified signal set */
#define	SIG_SETMASK	2	/* set specified signal set */

#if defined(_NETBSD_SOURCE)
typedef	void (*sig_t)(int);	/* type of signal function */
#endif

#if (defined(_XOPEN_SOURCE) && defined(_XOPEN_SOURCE_EXTENDED)) || \
    (_XOPEN_SOURCE - 0) >= 500 || defined(_NETBSD_SOURCE)
/*
 * Flags used with stack_t/struct sigaltstack.
 */
#define SS_ONSTACK	0x0001	/* take signals on alternate stack */
#define SS_DISABLE	0x0004	/* disable taking signals on alternate stack */
#ifdef _KERNEL
#define	SS_ALLBITS	0x0005
#endif
#define	MINSIGSTKSZ	8192			/* minimum allowable stack */
#define	SIGSTKSZ	(MINSIGSTKSZ + 32768)   /* recommended stack size */
#endif /* _XOPEN_SOURCE_EXTENDED || _XOPEN_SOURCE >= 500 || _NETBSD_SOURCE */

#if (defined(_XOPEN_SOURCE) && defined(_XOPEN_SOURCE_EXTENDED)) || \
    (_XOPEN_SOURCE - 0) >= 500 || defined(_NETBSD_SOURCE)
/*
 * Structure used in sigstack call.
 */
struct	sigstack {
	void	*ss_sp;			/* signal stack pointer */
	int	ss_onstack;		/* current status */
};
#endif /* _XOPEN_SOURCE_EXTENDED || _XOPEN_SOURCE >= 500 || _NETBSD_SOURCE */

#if defined(_NETBSD_SOURCE) && !defined(_KERNEL)
/*
 * Macro for converting signal number to a mask suitable for
 * sigblock().
 */
#define sigmask(n)	__sigmask(n)

#define	BADSIG		SIG_ERR
#endif /* _NETBSD_SOURCE */

#if (_POSIX_C_SOURCE - 0) >= 199309L || (_XOPEN_SOURCE - 0) >= 500 || \
    defined(_NETBSD_SOURCE)
struct	sigevent {
	int	sigev_notify;
	int	sigev_signo;
	union sigval	sigev_value;
	void	(*sigev_notify_function)(union sigval);
	void /* pthread_attr_t */	*sigev_notify_attributes;
};

#define SIGEV_SIGNAL	0
#define SIGEV_NONE	1
#define SIGEV_THREAD	2
#endif /* (_POSIX_C_SOURCE - 0) >= 199309L || ... */

#endif	/* _POSIX_C_SOURCE || _XOPEN_SOURCE || _NETBSD_SOURCE */

/*
 * For historical reasons; programs expect signal's return value to be
 * defined by <sys/signal.h>.
 */
__BEGIN_DECLS
void	(*signal(int, void (*)(int)))(int);
__END_DECLS
#endif	/* !_SYS_SIGNAL_H_ */
