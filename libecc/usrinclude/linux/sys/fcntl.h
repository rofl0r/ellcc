/*	$NetBSD: fcntl.h,v 1.36 2010/09/21 19:26:18 chs Exp $	*/

/*-
 * Copyright (c) 1983, 1990, 1993
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
 *	@(#)fcntl.h	8.3 (Berkeley) 1/21/94
 */

/* rdp - This file has been updated to contain the Linux definitions. */

#ifndef _SYS_FCNTL_H_
#define	_SYS_FCNTL_H_

/*
 * This file includes the definitions for open and fcntl
 * described by POSIX for <fcntl.h>; it also includes
 * related kernel definitions.
 */

#include <sys/featuretest.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 * File status flags: these are used by open(2), fcntl(2).
 * They are also used (indirectly) in the kernel file structure f_flags,
 * which is a superset of the open/fcntl flags.  Open flags and f_flags
 * are inter-convertible using OFLAGS(fflags) and FFLAGS(oflags).
 * Open/fcntl flags begin with O_; kernel-internal flags begin with F.
 */
/* open-only flags */
#define	O_RDONLY	0x00000000	/* open for reading only */
#define	O_WRONLY	0x00000001	/* open for writing only */
#define	O_RDWR		0x00000002	/* open for reading and writing */
#define	O_ACCMODE	0x00000003	/* mask for above modes */

/*
 * Kernel encoding of open mode; separate read and write bits that are
 * independently testable: 1 greater than the above.
 */
#define	O_CREAT		0x00000040	/* create if nonexistent */
#define	O_EXCL		0x00000080	/* error if already exists */
#define	O_NOCTTY	0x00000100	/* don't assign controlling terminal */
#define	O_TRUNC		0x00000200	/* truncate to zero length */
#define	O_APPEND	0x00000400	/* set append mode */
#define	O_NONBLOCK	0x00000800	/* no delay */
#define	O_NDELAY	O_NONBLOCK	/* compat */
#define	O_SYNC		0x00001000	/* synchronous writes */
#define	O_FSYNC		O_SYNC		/* compat */
#define	O_ASYNC		0x00002000	/* signal pgrp when data ready */

#ifdef __USE_GNU
#define	O_DIRECT	0x00004000	/* direct I/O hint */
#define	O_DIRECTORY	0x00010000	/* fail if not a directory */
#define	O_NOFOLLOW	0x00020000	/* don't follow symlinks on the last */
					/* path component */
#define O_NOATIME       0x00040000      /* Don't set atime */
#define O_CLOEXEC       0x00080000      /* Close on exec */
#endif

/* defined by POSIX 1003.1; BSD default, but required to be bitwise distinct */

#if (_POSIX_C_SOURCE - 0) >= 199309L || (_XOPEN_SOURCE - 0) >= 500 || \
    defined(_NETBSD_SOURCE)
#define	O_DSYNC		O_SYNC	        /* write: I/O data completion */
#define	O_RSYNC		O_SYNC	        /* read: I/O completion as for write */
#endif

/*
 * Constants used for fcntl(2)
 */

/* command values */
#define	F_DUPFD		0		/* duplicate file descriptor */
#define	F_GETFD		1		/* get file descriptor flags */
#define	F_SETFD		2		/* set file descriptor flags */
#define	F_GETFL		3		/* get file status flags */
#define	F_SETFL		4		/* set file status flags */
#define	F_GETLK		5		/* get record locking information */
#define	F_SETLK		6		/* set record locking information */
#define	F_SETLKW	7		/* F_SETLK; wait if blocked */
#define	F_SETOWN	8		/* set SIGIO/SIGURG proc/pgrp */
#define	F_GETOWN	9		/* get SIGIO/SIGURG proc/pgrp */

#define	F_GETLK64	12		/* get record locking information */
#define	F_SETLK64	13		/* set record locking information */
#define	F_SETLKW64	14		/* F_SETLK; wait if blocked */

#ifdef __USE_GNU
#define F_SETSIG        10              /* Set the signal number */
#define F_GETSIG        11              /* Get the signal number */
#define F_SETOWN_EX     15              /* Set owner */
#define F_GETOWN_EX     16              /* Get owner */

#define F_SETLEASE      1024            /* Set a lease */
#define F_GETLEASE      1025            /* Get the active lease */
#define F_NOTIFY        1026            /* Request directory notifications */
#define F_DUPFD_CLOEXEC 1030            /* Dup file descriptor with close-on-exec */
#endif

/* file descriptor flags (F_GETFD, F_SETFD) */
#define	FD_CLOEXEC	1		/* close-on-exec flag */

/* record locking flags (F_GETLK, F_SETLK, F_SETLKW) */
#define	F_RDLCK		0		/* shared or read lock */
#define	F_WRLCK		1		/* exclusive or write lock */
#define	F_UNLCK		2		/* unlock */
#define F_EXLCK         4
#define F_SHLCK         8

/*
 * Advisory file segment locking data type -
 * information passed to system by user
 */
struct flock {
	short	l_type;		/* lock type: read/write, etc. */
	short	l_whence;	/* type of l_start */
	off_t	l_start;	/* starting offset */
	off_t	l_len;		/* len = 0 means until end of file */
	pid_t	l_pid;		/* lock owner */
};

#ifdef __USE_BSD
/* lock operations for flock(2) */
#define	LOCK_SH		0x01		/* shared file lock */
#define	LOCK_EX		0x02		/* exclusive file lock */
#define	LOCK_NB		0x04		/* don't block when locking */
#define	LOCK_UN		0x08		/* unlock file */
#endif

#ifdef __USE_GNU
#define LOCK_MAND       0x20            /* Mandatory lock */
#define LOCK_READ       0x40            /* Allow concurrent reads */
#define LOCK_WRITE      0x80            /* Allow concurrent writes */
#define LOCK_RW         (LOCK_WRITE|LOCK_READ)

/* Directory notifications for F_NOTIFY */
#define DN_ACCESS       0x00000001      /* File accessed */
#define DN_MODIFY       0x00000002      /* File modified */
#define DN_CREATE       0x00000004      /* File created */
#define DN_DELETE       0x00000008      /* File deleted */
#define DN_RENAME       0x00000010      /* File renamed */
#define DN_ATTRIB       0x00000020      /* Changed file attributes */
#define DN_MULTISHOT    0x80000000      /* Don't remove nofitier */
#endif

#ifdef __USE_GNU
enum _pid_type {
    F_OWNER_TID,                /* Kernel thread */
    F_OWNER_PID,                /* Process */
    F_OWNER_GID                 /* Process group */
};
#endif

/* Always ensure that these are consistent with <stdio.h> and <unistd.h>! */
#ifndef	SEEK_SET
#define	SEEK_SET	0	/* set file offset to offset */
#endif
#ifndef	SEEK_CUR
#define	SEEK_CUR	1	/* set file offset to current plus offset */
#endif
#ifndef	SEEK_END
#define	SEEK_END	2	/* set file offset to EOF plus offset */
#endif

/*
 * posix_advise advisories.
 */

#define	POSIX_FADV_NORMAL	0	/* default advice / no advice */
#define	POSIX_FADV_RANDOM	1	/* random access */
#define	POSIX_FADV_SEQUENTIAL	2	/* sequential access(lower to higher) */
#define	POSIX_FADV_WILLNEED	3	/* be needed in near future */
#define	POSIX_FADV_DONTNEED	4	/* not be needed in near future */
#define	POSIX_FADV_NOREUSE	5	/* be accessed once */

#ifdef __USE_GNU
/* SYNC_FILE_RANGE flags */
#define SYNC_FILE_RANGE_WAIT_BEFORE     1       /* Wait before write */
#define SYNC_FILE_RANGE_WRITE           2       /* Write dirty pages */
#define SYNC_FILE_RANGE_WAIT_AFTER      4       /* Wait for write */

/* SPLICE and VMSPLICE flags */
#define SPLICE_F_MOVE           1       /* Move pages */
#define SPLICE_F_NONBLOCK       2       /* Don't block */
#define SPLICE_F_MORE           4       /* Expect more data */
#define SPLICE_F_GIFT           8       /* Pages are a gift */
#endif

#include <sys/cdefs.h>

__BEGIN_DECLS

int	open(const char *, int, ...);
int	creat(const char *, mode_t);
int	fcntl(int, int, ...);
int	flock(int, int);
int	posix_fadvise(int, off_t, off_t, int);
__END_DECLS

#endif /* !_SYS_FCNTL_H_ */
