#define _LOCORE
#include <machine/asm.h>
#include <machine/cpuregs.h>

#define code "ax", @progbits

        .set mips32r2

        .text
        .align 2
        .weak   _nmi_handler
        .ent    _nmi_handler
_nmi_handler:
        mfc0    k0, MIPS_COP_0_STATUS   // Get the status register.
        ins     k0, zero, MIPS_SR_BEV_BIT, 1
        mtc0    k0, MIPS_COP_0_STATUS   // Clear the boot exception vector bit.
        eret                            // Return from the exception.
        .end    _nmi_handler
