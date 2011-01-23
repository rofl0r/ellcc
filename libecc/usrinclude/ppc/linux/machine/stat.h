/*	$NetBSD: stat.h,v 1.59 2009/08/27 07:28:42 mouse Exp $	*/

/*-
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
 *	@(#)stat.h	8.12 (Berkeley) 8/17/94
 */

/* rdp - This file has been updated to contain the Linux definitions. */

#ifndef _MACHINE_STAT_H_
#define	_MACHINE_STAT_H_

#include <sys/featuretest.h>
#include <sys/types.h>
#include <sys/time.h>
#include <machine/stat.h>

struct stat {
	dev_t	  st_dev;		/* inode's device */
	ino_t	  st_ino;		/* inode's number */
	mode_t	  st_mode;		/* inode protection mode */
	nlink_t	  st_nlink;		/* number of hard links */
	uid_t	  st_uid;		/* user ID of the file's owner */
	gid_t	  st_gid;		/* group ID of the file's group */
	dev_t	  st_rdev;		/* device type */
	off_t	  st_size;		/* file size, in bytes */
	blksize_t st_blksize;		/* optimal blocksize for I/O */
	blkcnt_t  st_blocks;		/* blocks allocated for file */
	struct	  timespec st_atimespec;/* time of last access */
	struct	  timespec st_mtimespec;/* time of last data modification */
	struct	  timespec st_ctimespec;/* time of last file status change */
        int32_t   pad1[12];
};

#endif // _MACHINE_STAT_H_
