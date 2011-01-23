/*	$NetBSD: setjmp.h,v 1.3 2008/08/29 19:08:29 matt Exp $	*/

/*
 * machine/setjmp.h: machine dependent setjmp-related information.
 */

#define	_JBLEN	28		/* size, in longs, of a jmp_buf */

/*
 * NOTE: The internal structure of a jmp_buf is *PRIVATE*
 *       This information is provided as there is software
 *       that fiddles with this with obtain the stack pointer
 *	 (yes really ! and its commercial !).
 *
 * Description of the setjmp buffer
 *
 * word  0 -  2	f4		fp register 4
 *	 3 -  5	f5		fp register 5
 *	 6 -  8 f6		fp register 6
 *	 9 - 11	f7		fp register 7
 *	12	fpsr		fp status register
 *	13	r4		register 4
 *	14	r5		register 5
 *	15	r6		register 6
 *	16	r7		register 7
 *	17	r8		register 8
 *	18	r9		register 9
 *	19	r10		register 10 (sl)
 *	20	r11		register 11 (fp)
 *	21	r12		register 12 (ip)
 *	22	r13		register 13 (sp)
 *	23	r14		register 14 (lr)
 *	24	signal mask	(dependant on magic)
 *	25	(con't)
 *	26	(con't)
 *	27	(con't)
 *
 * A side note I should mention - Please do not tamper
 * with the floating point fields. While they are
 * always saved and restored at the moment this cannot
 * be guaranteed especially if the compiler happens
 * to be generating soft-float code so no fp
 * registers will be used.
 *
 * Whilst this can be seen an encouraging people to
 * use the setjmp buffer in this way I think that it
 * is for the best then if changes occur compiles will
 * break rather than just having new builds falling over
 * mysteriously.
 */

/* Valid for all jmp_buf's */

#define _JB_REG_F4		 0
#define _JB_REG_F5		 3
#define _JB_REG_F6		 6
#define _JB_REG_F7		 9
#define _JB_REG_FPSR		12
#define _JB_REG_R4		13
#define _JB_REG_R5		14
#define _JB_REG_R6		15
#define _JB_REG_R7		16
#define _JB_REG_R8		17
#define _JB_REG_R9		18
#define _JB_REG_R10		19
#define _JB_REG_R11		20
#define _JB_REG_R12		21
#define _JB_REG_R13		22
#define _JB_REG_R14		23
#define _JB_SIGMASK		24
