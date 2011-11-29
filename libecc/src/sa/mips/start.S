#define _LOCORE
#include <machine/asm.h>
#include <machine/cpuregs.h>

/** The initial entry point.
 */
        .text
/** The beginning of the .text section is given on the command line for stand
 * alone programs. For the Mips, the reset address is 0xbfc00000. The
 * ecc command line should contain "-Wl,-Ttext,bfc00000,-Tdata,a0000010".
 */
        .set    noreorder
        mfc0    k0, MIPS_COP_0_STATUS
        li      k1, MIPS3_SR_NMI
        and     k0, k0, k1              // Check for an NMI.
        beqz    k0, _start              // Jump if reset.
        nop
        la      k0, _nmi                // Vector to the NMI handler.
        jr      k0
        nop

_start:
        la      sp, _stack              // Set up the stack pointer.
        la      gp, _gp                 // Initialize the global pointer.

        lw      a0, (sp)                // Get argc...
        nop
        addu    a1, sp, 4               // ... and argv ...
        addi    a2, a0, 1               // a2 = argc + 1
        addu    a2, a2, a2
        addu    a2, a2, a2              // ... a2 = argc * 4 ...
        addu    a2, a2, a1              // ... and compute environ.
        lw      t9, %call16(_estart)(gp)
        jalr    t9                      // Let's go!
        nop
        b       .                       // Never gets here.
        nop
