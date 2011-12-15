/*	$NetBSD: signame.c,v 1.13 2005/09/13 01:44:09 christos Exp $	*/

/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
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
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char sccsid[] = "from: @(#)siglist.c	5.6 (Berkeley) 2/23/91";*/
#else
__RCSID("$NetBSD: signame.c,v 1.13 2005/09/13 01:44:09 christos Exp $");
#endif
#endif /* LIBC_SCCS and not lint */

#include <signal.h>
#include <unistd.h>

static const char *const __signame[] = {
	"Signal 0",	/* 0 */
	"HUP",		/* 1 SIGHUP */
	"INT",		/* 2 SIGINT */
	"QUIT",		/* 3 SIGQUIT */
	"ILL",		/* 4 SIGILL */
	"TRAP",		/* 5 SIGTRAP */
	"ABRT",		/* 6 SIGABRT */
	"BUS",		/* 7 SIGBUS */
	"FPE",		/* 8 SIGFPE */
	"KILL",		/* 9 SIGKILL */
	"USR1",		/* 10 SIGUSR1 */
	"SEGV",		/* 11 SIGSEGV */
	"USR2",		/* 12 SIGUSR2 */
	"PIPE",		/* 13 SIGPIPE */
	"ALRM",		/* 14 SIGALRM */
	"TERM",		/* 15 SIGTERM */
	"STKFLT",	/* 16 SIGSTKFLT */
	"CHLD",		/* 17 SIGCHLD */
	"CONT",		/* 18 SIGCONT */
	"STOP",		/* 19 SIGSTOP */
	"TSTP",		/* 20 SIGTSTP */
	"TTIN",		/* 21 SIGTTIN */
	"TTOU",		/* 22 SIGTTOU */
	"URG",		/* 23 SIGURG */
	"XCPU",		/* 24 SIGXCPU */
	"XFSZ",		/* 25 SIGXFSZ */
	"VTALRM",	/* 26 SIGVTALRM */
	"PROF",		/* 27 SIGPROF */
	"WINCH",	/* 28 SIGWINCH */
	"IO",		/* 29 SIGIO */
	"PWR",		/* 30 SIGPWR */
	"SYS",		/* 31 SIGSYS */
};

const char * const *__sys_signame = __signame;
extern const char *const *sys_signame __strong_alias(__sys_signame);
