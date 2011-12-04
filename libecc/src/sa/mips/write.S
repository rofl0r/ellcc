#define _LOCORE
#include <machine/asm.h>
#include <machine/cpuregs.h>
#include <asm/unistd.h>

        .text
        .ent    write
write:
        li      v0, __NR_write
        syscall
        .end    write
        .globl  write
