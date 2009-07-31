/* Copyright (C) 1997, 2002, 2003, 2004, 2006, 2008
   Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Miguel de Icaza <miguel@gnu.ai.mit.edu>, January 1997.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef _LINUX_SPARC32_SYSDEP_H
#define _LINUX_SPARC32_SYSDEP_H 1

#include <unix/sparc/sysdep.h>

#ifdef IS_IN_rtld
# include <dl-sysdep.h>		/* Defines RTLD_PRIVATE_ERRNO.  */
#endif
// RICH: #include <tls.h>

#undef SYS_ify
#define SYS_ify(syscall_name) __NR_##syscall_name

#ifdef __ASSEMBLER__

#define LOADSYSCALL(x) mov __NR_##x, %g1

/* Linux/SPARC uses a different trap number */
#undef PSEUDO
#undef PSEUDO_NOERRNO
#undef PSEUDO_ERRVAL
#undef PSEUDO_END
#undef ENTRY
#undef END
#undef LOC

#define ENTRY(name)			\
	.align	4;			\
	.global	C_SYMBOL_NAME(name);	\
	.type	name, @function;	\
C_LABEL(name)				\
	cfi_startproc;

#define END(name)			\
	cfi_endproc;			\
	.size name, . - name

#define LOC(name)  .L##name

	/* If the offset to __syscall_error fits into a signed 22-bit
	 * immediate branch offset, the linker will relax the call into
	 * a normal branch.
	 */
#define PSEUDO(name, syscall_name, args)	\
	.text;					\
	.globl		__syscall_error;	\
ENTRY(name);					\
	LOADSYSCALL(syscall_name);		\
	ta		0x10;			\
	bcc		1f;			\
	 mov		%o7, %g1;		\
	call		__syscall_error;	\
	 mov		%g1, %o7;		\
1:

#define PSEUDO_NOERRNO(name, syscall_name, args)\
	.text;					\
ENTRY(name);					\
	LOADSYSCALL(syscall_name);		\
	ta		0x10;

#define PSEUDO_ERRVAL(name, syscall_name, args)	\
	.text;					\
ENTRY(name);					\
	LOADSYSCALL(syscall_name);		\
	ta		0x10;

#define PSEUDO_END(name)			\
	END(name)

#else  /* __ASSEMBLER__ */

#if defined SHARED && defined DO_VERSIONING && defined PIC \
    && !defined NO_HIDDEN && !defined NOT_IN_libc
# define CALL_ERRNO_LOCATION "call   __GI___errno_location;"
#else
# define CALL_ERRNO_LOCATION "call   __errno_location;"
#endif

#define __SYSCALL_STRING						\
	"ta	0x10;"							\
	"bcs	2f;"							\
	" nop;"								\
	"1:"								\
	".subsection 2;"						\
	"2:"								\
	"save	%%sp, -192, %%sp;"					\
	CALL_ERRNO_LOCATION						\
	" nop;"								\
	"st	%%i0,[%%o0];"						\
	"ba	1b;"							\
	" restore %%g0, -1, %%o0;"					\
	".previous;"

#define __CLONE_SYSCALL_STRING						\
	"ta	0x10;"							\
	"bcs	2f;"							\
	" sub	%%o1, 1, %%o1;"						\
	"and	%%o0, %%o1, %%o0;"					\
	"1:"								\
	".subsection 2;"						\
	"2:"								\
	"save	%%sp, -192, %%sp;"					\
	CALL_ERRNO_LOCATION						\
	" nop;"								\
	"st	%%i0, [%%o0];"						\
	"ba	1b;"							\
	" restore %%g0, -1, %%o0;"					\
	".previous;"

#define __INTERNAL_SYSCALL_STRING					\
	"ta	0x10;"							\
	"bcs,a	1f;"							\
	" sub	%%g0, %%o0, %%o0;"					\
	"1:"

#define __SYSCALL_CLOBBERS						\
	"f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",			\
	"f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15",		\
	"f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",		\
	"f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31",		\
	"cc", "memory"

#undef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...) \
  inline_syscall##nr(__SYSCALL_STRING, __NR_##name, args)

#undef INTERNAL_SYSCALL_DECL
#define INTERNAL_SYSCALL_DECL(err) do { } while (0)

#undef INTERNAL_SYSCALL
#define INTERNAL_SYSCALL(name, err, nr, args...) \
  inline_syscall##nr(__INTERNAL_SYSCALL_STRING, __NR_##name, args)

#undef INTERNAL_SYSCALL_NCS
#define INTERNAL_SYSCALL_NCS(name, err, nr, args...) \
  inline_syscall##nr(__INTERNAL_SYSCALL_STRING, name, args)

#undef INTERNAL_SYSCALL_ERROR_P
#define INTERNAL_SYSCALL_ERROR_P(val, err) \
  ((unsigned long) (val) >= -515L)

#undef INTERNAL_SYSCALL_ERRNO
#define INTERNAL_SYSCALL_ERRNO(val, err)	(-(val))

#define inline_syscall0(string,name,dummy...)				\
({									\
	register long __o0 __asm__ ("o0");				\
	register long __g1 __asm__ ("g1") = name;			\
	__asm __volatile (string : "=r" (__g1), "=r" (__o0) :		\
			  "0" (__g1) :					\
			  __SYSCALL_CLOBBERS);				\
	__o0;								\
})

#define inline_syscall1(string,name,arg1)				\
({									\
	register long __o0 __asm__ ("o0") = (long)(arg1);		\
	register long __g1 __asm__ ("g1") = name;			\
	__asm __volatile (string : "=r" (__g1), "=r" (__o0) :		\
			  "0" (__g1), "1" (__o0) :			\
			  __SYSCALL_CLOBBERS);				\
	__o0;								\
})

#define inline_syscall2(string,name,arg1,arg2)				\
({									\
	register long __o0 __asm__ ("o0") = (long)(arg1);		\
	register long __o1 __asm__ ("o1") = (long)(arg2);		\
	register long __g1 __asm__ ("g1") = name;			\
	__asm __volatile (string : "=r" (__g1), "=r" (__o0) :		\
			  "0" (__g1), "1" (__o0), "r" (__o1) :		\
			  __SYSCALL_CLOBBERS);				\
	__o0;								\
})

#define inline_syscall3(string,name,arg1,arg2,arg3)			\
({									\
	register long __o0 __asm__ ("o0") = (long)(arg1);		\
	register long __o1 __asm__ ("o1") = (long)(arg2);		\
	register long __o2 __asm__ ("o2") = (long)(arg3);		\
	register long __g1 __asm__ ("g1") = name;			\
	__asm __volatile (string : "=r" (__g1), "=r" (__o0) :		\
			  "0" (__g1), "1" (__o0), "r" (__o1),		\
			  "r" (__o2) :					\
			  __SYSCALL_CLOBBERS);				\
	__o0;								\
})

#define inline_syscall4(string,name,arg1,arg2,arg3,arg4)		\
({									\
	register long __o0 __asm__ ("o0") = (long)(arg1);		\
	register long __o1 __asm__ ("o1") = (long)(arg2);		\
	register long __o2 __asm__ ("o2") = (long)(arg3);		\
	register long __o3 __asm__ ("o3") = (long)(arg4);		\
	register long __g1 __asm__ ("g1") = name;			\
	__asm __volatile (string : "=r" (__g1), "=r" (__o0) :		\
			  "0" (__g1), "1" (__o0), "r" (__o1),		\
			  "r" (__o2), "r" (__o3) :			\
			  __SYSCALL_CLOBBERS);				\
	__o0;								\
})

#define inline_syscall5(string,name,arg1,arg2,arg3,arg4,arg5)		\
({									\
	register long __o0 __asm__ ("o0") = (long)(arg1);		\
	register long __o1 __asm__ ("o1") = (long)(arg2);		\
	register long __o2 __asm__ ("o2") = (long)(arg3);		\
	register long __o3 __asm__ ("o3") = (long)(arg4);		\
	register long __o4 __asm__ ("o4") = (long)(arg5);		\
	register long __g1 __asm__ ("g1") = name;			\
	__asm __volatile (string : "=r" (__g1), "=r" (__o0) :		\
			  "0" (__g1), "1" (__o0), "r" (__o1),		\
			  "r" (__o2), "r" (__o3), "r" (__o4) :		\
			  __SYSCALL_CLOBBERS);				\
	__o0;								\
})

#define inline_syscall6(string,name,arg1,arg2,arg3,arg4,arg5,arg6)	\
({									\
	register long __o0 __asm__ ("o0") = (long)(arg1);		\
	register long __o1 __asm__ ("o1") = (long)(arg2);		\
	register long __o2 __asm__ ("o2") = (long)(arg3);		\
	register long __o3 __asm__ ("o3") = (long)(arg4);		\
	register long __o4 __asm__ ("o4") = (long)(arg5);		\
	register long __o5 __asm__ ("o5") = (long)(arg6);		\
	register long __g1 __asm__ ("g1") = name;			\
	__asm __volatile (string : "=r" (__g1), "=r" (__o0) :		\
			  "0" (__g1), "1" (__o0), "r" (__o1),		\
			  "r" (__o2), "r" (__o3), "r" (__o4),		\
			  "r" (__o5) :					\
			  __SYSCALL_CLOBBERS);				\
	__o0;								\
})

#define INLINE_CLONE_SYSCALL(arg1,arg2,arg3,arg4,arg5)			\
({									\
	register long __o0 __asm__ ("o0") = (long)(arg1);		\
	register long __o1 __asm__ ("o1") = (long)(arg2);		\
	register long __o2 __asm__ ("o2") = (long)(arg3);		\
	register long __o3 __asm__ ("o3") = (long)(arg4);		\
	register long __o4 __asm__ ("o4") = (long)(arg5);		\
	register long __g1 __asm__ ("g1") = __NR_clone;			\
	__asm __volatile (__CLONE_SYSCALL_STRING :			\
			  "=r" (__g1), "=r" (__o0), "=r" (__o1)	:	\
			  "0" (__g1), "1" (__o0), "2" (__o1),		\
			  "r" (__o2), "r" (__o3), "r" (__o4) :		\
			  __SYSCALL_CLOBBERS);				\
	__o0;								\
})


#ifdef __ASSEMBLER__
# define JUMPTARGET(sym) sym
#endif


#endif	/* __ASSEMBLER__ */

/* Pointer mangling support.  */
#if defined NOT_IN_libc && defined IS_IN_rtld
/* We cannot use the thread descriptor because in ld.so we use setjmp
   earlier than the descriptor is initialized.  */
#else
# ifdef __ASSEMBLER__
#  define PTR_MANGLE(dreg, reg, tmpreg) \
  ld	[%g7 + POINTER_GUARD], tmpreg; \
  xor	reg, tmpreg, dreg
#  define PTR_DEMANGLE(dreg, reg, tmpreg) PTR_MANGLE (dreg, reg, tmpreg)
#  define PTR_MANGLE2(dreg, reg, tmpreg) \
  xor	reg, tmpreg, dreg
#  define PTR_DEMANGLE2(dreg, reg, tmpreg) PTR_MANGLE2 (dreg, reg, tmpreg)
# else
#  define PTR_MANGLE(var) \
  (var) = (__typeof (var)) ((uintptr_t) (var) ^ THREAD_GET_POINTER_GUARD ())
#  define PTR_DEMANGLE(var)     PTR_MANGLE (var)
# endif
#endif

#endif /* linux/sparc/sysdep.h */
