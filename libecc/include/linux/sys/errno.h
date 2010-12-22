/*	$NetBSD: errno.h,v 1.39 2006/10/31 00:38:07 cbiere Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1989, 1993
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
 *	@(#)errno.h	8.5 (Berkeley) 1/21/94
 */

/* rdp - This file has been updated to contain the Linux error numbers. */

#ifndef _SYS_ERRNO_H_
#define _SYS_ERRNO_H_

#define	EPERM		1		/* Operation not permitted */
#define	ENOENT		2		/* No such file or directory */
#define	ESRCH		3		/* No such process */
#define	EINTR		4		/* Interrupted system call */
#define	EIO		5		/* Input/output error */
#define	ENXIO		6		/* Device not configured */
#define	E2BIG		7		/* Argument list too long */
#define	ENOEXEC		8		/* Exec format error */
#define	EBADF		9		/* Bad file descriptor */
#define	ECHILD		10		/* No child processes */
#define	EAGAIN		11		/* Resource temporarily unavailable */
#define	EWOULDBLOCK	EAGAIN		/* Operation would block */
#define	ENOMEM		12		/* Cannot allocate memory */
#define	EACCES		13		/* Permission denied */
#define	EFAULT		14		/* Bad address */
#define	ENOTBLK		15		/* Block device required */
#define	EBUSY		16		/* Device busy */
#define	EEXIST		17		/* File exists */
#define	EXDEV		18		/* Cross-device link */
#define	ENODEV		19		/* Operation not supported by device */
#define	ENOTDIR		20		/* Not a directory */
#define	EISDIR		21		/* Is a directory */
#define	EINVAL		22		/* Invalid argument */
#define	ENFILE		23		/* Too many open files in system */
#define	EMFILE		24		/* Too many open files */
#define	ENOTTY		25		/* Inappropriate ioctl for device */
#define	ETXTBSY		26		/* Text file busy */
#define	EFBIG		27		/* File too large */
#define	ENOSPC		28		/* No space left on device */
#define	ESPIPE		29		/* Illegal seek */
#define	EROFS		30		/* Read-only file system */
#define	EMLINK		31		/* Too many links */
#define	EPIPE		32		/* Broken pipe */

/* math software */
#define	EDOM		33		/* Numerical argument out of domain */
#define	ERANGE		34		/* Result too large or too small */

#define	EDEADLK		35		/* Resource deadlock avoided */
#define EDEADLOCK       EDEADLK
#define	ENAMETOOLONG	36		/* File name too long */
#define	ENOLCK		37		/* No locks available */
#define	ENOSYS		38		/* Function not implemented */
#define	ENOTEMPTY	39		/* Directory not empty */
#define	ELOOP		40		/* Too many levels of symbolic links */
#define	ENOMSG		42		/* No message of desired type */
#define	EIDRM		43		/* Identifier removed */
#define ECHRNG          44              /* Channel number out of range */
#define EL2NSYNC        45              /* Level 2 not synchronized */
#define EL3HLT          46              /* Level 3 halted */
#define EL3RST          47              /* Level 3 reset */
#define ELNRNG          48              /* Link out of range */
#define EUNATCH         49              /* Protocol driver not attached */
#define ENOCSI          50              /* No CSI structure available */
#define EL2HLT          51              /* Level 2 halted */
#define EBADE           52              /* Invalid exchange */
#define EBADR           53              /* Invalid request descriptor */
#define EXFULL          54              /* Exchange full */
#define ENOANO          55              /* No anode */
#define EBADRQC         56              /* Invalid request */
#define EBADSLT         57              /* Invalid slot */
#define EBFONT          59              /* Invalid font file format */
#define ENOSTR		60		/* Not a STREAM */
#define ENODATA		61		/* No message available */
#define ETIME		62		/* STREAM ioctl timeout */
#define ENOSR		63		/* No STREAM resources */
#define ENONET          64              /* Not on the network */
#define ENOPKG          65              /* Package not installed */
#define	EREMOTE		66		/* Too many levels of remote in path */
#define	ENOLINK		67		/* Link has been severed */
#define EADV            68              /* Advertizse error */
#define ESRMNT          69              /* Srmount error */
#define ECOMM           70              /* Communication error */
#define	EPROTO		71		/* Protocol error */
#define	EMULTIHOP	72		/* Multihop attempted */ 
#define EDOTDOT         73              /* RFS specific error */
#define EBADMSG		74		/* Bad or Corrupt message */
#define	EOVERFLOW	75		/* Value too large to be stored in data type */
#define ENOTUNIQ        76              /* Network name not unique */
#define EBADFD          77              /* Bad file descriptor */
#define EREMCHG         78              /* Remote address changed */
#define ELIBACC         79              /* Can't access shared library */
#define ELIBBAD         80              /* Bad shared library */
#define ELIBSCN         81              /* Bad .lib section */
#define ELIBMAX         82              /* Too many shared libraries */
#define ELIBEXEC        83              /* Can't execute a shared library */
#define	EILSEQ		84		/* Illegal byte sequence */
#define	ERESTART	85		/* restart syscall */
#define ESTRPIPE        86              /* Streams pipe error */
#define	EUSERS		87		/* Too many users */
#define	ENOTSOCK	88		/* Socket operation on non-socket */
#define	EDESTADDRREQ	89		/* Destination address required */
#define	EMSGSIZE	90		/* Message too long */
#define	EPROTOTYPE	91		/* Protocol wrong type for socket */
#define	ENOPROTOOPT	92		/* Protocol option not available */
#define	EPROTONOSUPPORT	93		/* Protocol not supported */
#define	ESOCKTNOSUPPORT	94		/* Socket type not supported */
#define	EOPNOTSUPP	95		/* Operation not supported */
#define	EPFNOSUPPORT	96		/* Protocol family not supported */
#define	EAFNOSUPPORT	97		/* Address family not supported by protocol family */
#define	EADDRINUSE	98		/* Address already in use */
#define	EADDRNOTAVAIL	99		/* Can't assign requested address */
#define	ENETDOWN	100		/* Network is down */
#define	ENETUNREACH	101		/* Network is unreachable */
#define	ENETRESET	102		/* Network dropped connection on reset */
#define	ECONNABORTED	103		/* Software caused connection abort */
#define	ECONNRESET	104		/* Connection reset by peer */
#define	ENOBUFS		105		/* No buffer space available */
#define	EISCONN		106		/* Socket is already connected */
#define	ENOTCONN	107		/* Socket is not connected */
#define	ESHUTDOWN	108		/* Can't send after socket shutdown */
#define	ETOOMANYREFS	109		/* Too many references: can't splice */
#define	ETIMEDOUT	110		/* Operation timed out */
#define	ECONNREFUSED	111		/* Connection refused */
#define	EHOSTDOWN	112		/* Host is down */
#define	EHOSTUNREACH	113		/* No route to host */
#define	EALREADY	114		/* Operation already in progress */
#define	EINPROGRESS	115		/* Operation now in progress */
#define	ESTALE		116		/* Stale NFS file handle */
#define EUCLEAN         117             /* Structure needs cleaning */
#define ENOTNAM         118             /* Not a XENIX named type file */
#define ENAVAIL         119             /* No XENIX semaphores available */
#define EISNAM          120             /* A named type file */
#define EREMOTEIO       121             /* Remote I/O error */
#define	EDQUOT		122		/* Disc quota exceeded */
#define ENOMEDIUM       123             /* No medium found */
#define EMEDIUMTYPE     124             /* Bad medium type */
#define ECANCELED	125		/* Operation canceled */
#define ENOKEY          126             /* Key not available */
#define EKEYEXPIRED     127             /* Key expired */
#define EKEYREVOKED     128             /* Key revoked */
#define EKEYREJECTED    129             /* Key rejected */
#define EOWNERDEAD      130             /* Mutex owner died */
#define ENOTRECOVERABLE 131             /* State not recoverable */
#define ERFKILL         132             /* Operation not possible */

#endif /* !_SYS_ERRNO_H_ */
