/*	$NetBSD: sockio.h,v 1.30 2010/11/15 22:42:36 pooka Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1990, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)sockio.h	8.1 (Berkeley) 3/28/94
 */

#ifndef	_SYS_SOCKIO_H_
#define	_SYS_SOCKIO_H_

#include <sys/ioccom.h>

/* Socket ioctl's. */
#define	SIOCSPGRP	 0x8902                         /* set process group */
#define	SIOCGPGRP	 0x8904                         /* get process group */
#define	SIOCATMARK	 0x8905                         /* at oob mark? */

#define	SIOCADDRT	 0x890B                         /* add route */
#define	SIOCDELRT	 0x890C                         /* delete route */

#define	SIOCSIFADDR	 0x8916                         /* set ifnet address */
#define	SIOCGIFADDR	 0x8915                         /* get ifnet address */

#define	SIOCSIFDSTADDR	 0x8918                         /* set p-p address */
#define	SIOCGIFDSTADDR	 0x8917                         /* get p-p address */

#define	SIOCSIFFLAGS	 0x8914                         /* set ifnet flags */
#define	SIOCGIFFLAGS	 0x8913                         /* get ifnet flags */

#define	SIOCGIFBRDADDR	 0x8919                         /* get broadcast addr */
#define	SIOCSIFBRDADDR	 0x891a                         /* set broadcast addr */

#define	SIOCGIFCONF	 0x8912                         /* get ifnet list */

#define	SIOCGIFNETMASK	 0x891b                         /* get net addr mask */
#define	SIOCSIFNETMASK	 0x891c                         /* set net addr mask */

#define	SIOCGIFMETRIC	 0x891d                         /* get IF metric */
#define	SIOCSIFMETRIC	 0x891e                         /* set IF metric */

#define	SIOCDIFADDR	 0x8936                         /* delete IF addr */

#define	SIOCADDMULTI	 0x8931                         /* add m'cast addr */
#define	SIOCDELMULTI	 0x8932                         /* del m'cast addr */

#define	SIOCGETVIFCNT	 0x89e0                         /* vif pkt cnt */
#define	SIOCGETSGCNT	 0x89e1                         /* sg pkt cnt */

#define	SIOCSIFMTU	 0x8922                         /* set ifnet mtu */
#define	SIOCGIFMTU	 0x8921                         /* get ifnet mtu */

#endif /* !_SYS_SOCKIO_H_ */
