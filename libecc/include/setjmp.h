/*
	setjmp.h
	stubs for future use.
*/

#ifndef _SETJMP_H_
#define _SETJMP_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <machine/setjmp.h>

void longjmp(jmp_buf __jmpb, int __retval);
int setjmp(jmp_buf __jmpb);

#ifdef __cplusplus
}
#endif

#endif

