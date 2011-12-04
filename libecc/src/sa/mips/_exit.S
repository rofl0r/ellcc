#define _LOCORE
#include <machine/asm.h>
#include <machine/cpuregs.h>
#include <asm/unistd.h>

        .text
        .ent    _exit
_exit:
        li      v0, __NR_exit
        syscall
        .end    _exit
        .globl  _exit
