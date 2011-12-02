#define _LOCORE
#include <machine/asm.h>
#include <machine/cpuregs.h>
#include <asm/unistd.h>

        .text
_exit:
        syscall __NR_write
