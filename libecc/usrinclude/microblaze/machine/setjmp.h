/*	$NetBSD: setjmp.h,v 1.3 2008/08/29 19:08:29 matt Exp $	*/

/*
 * machine/setjmp.h: machine dependent setjmp-related information.
 */

#define	_JBLEN	16		/* size, in longs, of a jmp_buf */

#define _JB_REG_R1		 0
#define _JB_REG_R15		 1
#define _JB_REG_R18		 2
#define _JB_REG_R19		 3
#define _JB_REG_R20		 4
#define _JB_REG_R21		 5
#define _JB_REG_R22		 6
#define _JB_REG_R23		 7
#define _JB_REG_R24		 8
#define _JB_REG_R25		 9
#define _JB_REG_R26		10
#define _JB_REG_R27		11
#define _JB_REG_R28		12
#define _JB_REG_R29		13
#define _JB_REG_R30		14
#define _JB_SIGMASK		15
