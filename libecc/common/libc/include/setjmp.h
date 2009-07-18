/*
	setjmp.h
	stubs for future use.
*/

#ifndef _SETJMP_H_
#define _SETJMP_H_

#include "_ansi.h"
_BEGIN_STD_C
#include <machine/setjmp.h>

void longjmp(jmp_buf __jmpb, int __retval);
int setjmp(jmp_buf __jmpb);

_END_STD_C

#endif

