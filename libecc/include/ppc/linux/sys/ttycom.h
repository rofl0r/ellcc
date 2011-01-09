/*	$NetBSD: ttycom.h,v 1.18 2005/12/11 12:25:21 christos Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1990, 1993, 1994
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
 *	@(#)ttycom.h	8.1 (Berkeley) 3/28/94
 */

#ifndef	_SYS_TTYCOM_H_
#define	_SYS_TTYCOM_H_

#include <sys/ioccom.h>

/*
 * Tty ioctl's except for those supported only for backwards compatibility
 * with the old tty driver.
 */

/*
 * Window/terminal size structure.  This information is stored by the kernel
 * in order to provide a consistent interface, but is not used by the kernel.
 */
struct winsize {
	unsigned short	ws_row;		/* rows, in characters */
	unsigned short	ws_col;		/* columns, in characters */
	unsigned short	ws_xpixel;	/* horizontal size, pixels */
	unsigned short	ws_ypixel;	/* vertical size, pixels */
};

/* ptmget, for /dev/ptm pty getting ioctl PTMGET */
struct ptmget {
	int	cfd;
	int	sfd;
	char	cn[16];
	char	sn[16];
};

#define _PATH_PTMDEV	"/dev/ptm"

#define		TIOCM_LE	0001		/* line enable */
#define		TIOCM_DTR	0002		/* data terminal ready */
#define		TIOCM_RTS	0004		/* request to send */
#define		TIOCM_ST	0010		/* secondary transmit */
#define		TIOCM_SR	0020		/* secondary receive */
#define		TIOCM_CTS	0040		/* clear to send */
#define		TIOCM_CAR	0100		/* carrier detect */
#define		TIOCM_CD	TIOCM_CAR
#define		TIOCM_RNG	0200		/* ring */
#define		TIOCM_RI	TIOCM_RNG
#define		TIOCM_DSR	0400		/* data set ready */

#define	TIOCEXCL	0x540C                  /* set exclusive use of tty */
#define	TIOCNXCL        0x540D                  /* reset exclusive use of tty */
#define	TIOCFLUSH	0x540B                  /* flush buffers */
#define	TIOCGETA	0x5405  // RICH_IOR('t', 23, struct termio)    /* get termios struct */
#define	TIOCSETA	0x5406  // RICH_IOW('t', 24, struct termio)    /* set termios struct */
#define	TIOCSETAW	0x5407                  /* drain output, set */
#define	TIOCSETAF	0x5408                  /* drn out, fls in, set */

#define	TIOCSBRK	0x5427                  /* set break bit */
#define	TIOCCBRK	0x5428                  /* clear break bit */
#define	TIOCGPGRP	0x540F                  /* get pgrp of tty */
#define	TIOCSPGRP	0x5410  	        /* set pgrp of tty */
						/* 117-116 compat */
#define	TIOCOUTQ	0x5411                  /* output queue size */
#define	TIOCSTI		0x5412                  /* simulate terminal input */
#define	TIOCNOTTY	0x5422                  /* void tty association */
#define	TIOCPKT		0x5420                  /* pty: set/clear packet mode */
#define		TIOCPKT_DATA		0x00	/* data packet */
#define		TIOCPKT_FLUSHREAD	0x01	/* flush packet */
#define		TIOCPKT_FLUSHWRITE	0x02	/* flush packet */
#define		TIOCPKT_STOP		0x04	/* stop output */
#define		TIOCPKT_START		0x08	/* start output */
#define		TIOCPKT_NOSTOP		0x10	/* no more ^S, ^Q */
#define		TIOCPKT_DOSTOP		0x20	/* now do ^S ^Q */
#define	TIOCMSET	0x5418                  /* set all modem bits */
#define	TIOCMBIS	0x5416                  /* bis modem bits */
#define	TIOCMBIC	0x5417                  /* bic modem bits */
#define	TIOCMGET	0x5415                  /* get all modem bits */
#define	TIOCGWINSZ	0x5413                  /* get window size */
#define	TIOCSWINSZ	0x5414                  /* set window size */
#define	TIOCGSID	0x5429                  /* get session id */
#define	TIOCCONS	0x541D                  /* become virtual console */
#define	TIOCSCTTY	0x540E                  /* become controlling tty */

#endif /* !_SYS_TTYCOM_H_ */
