/*	$NetBSD: setjmp.h,v 1.3 2008/08/29 19:08:29 matt Exp $	*/

/*
 * machine/setjmp.h: machine dependent setjmp-related information.
 */

#define	_JBLEN	17		/* size, in longs, of a jmp_buf */

/*
 * NOTE: The internal structure of a jmp_buf is *PRIVATE*
 *       This information is provided as there is software
 *       that fiddles with this with obtain the stack pointer
 *	 (yes really ! and its commercial !).
 *
 * Description of the setjmp buffer
 *
 * word  0	magic number	(dependant on creator)
 *       1     	r1		register 1 (sp)
 *	 2     	r15		register 15 (lr)
 *	 3      r18		register 18
 *       4      r19             register 19
 *       5      r20             register 20
 *       6      r21             register 21
 *       7      r22             register 22
 *       8      r23             register 23
 *       9      r24             register 24
 *      10      r25             register 25
 *      11      r26             register 26
 *      12      r27             register 27
 *      13      r28             register 28
 *      14      r29             register 29
 *      15      r30             register 30
 *      16      signal mask
 *
 * The magic number number identifies the jmp_buf and
 * how the buffer was created as well as providing
 * a sanity check
 */

#define _JB_MAGIC_SETJMP	0x4278f500
#define _JB_MAGIC_MASK	        0xffffff00
#define _JB_MAGIC_BITS	        0x000000ff
#define _JB_MAGIC_SIGMASK	0x01

/* Valid for all jmp_buf's */

#define _JB_MAGIC		 0
#define _JB_REG_R1		 1
#define _JB_REG_R15		 2
#define _JB_REG_R18		 3
#define _JB_REG_R19		 4
#define _JB_REG_R20		 5
#define _JB_REG_R21		 6
#define _JB_REG_R22		 7
#define _JB_REG_R23		 8
#define _JB_REG_R24		 9
#define _JB_REG_R25		10
#define _JB_REG_R26		11
#define _JB_REG_R27		12
#define _JB_REG_R28		13
#define _JB_REG_R29		14
#define _JB_REG_R30		15

/* Only valid with the _JB_MAGIC_SETJMP bit */

#define _JB_SIGMASK		16
