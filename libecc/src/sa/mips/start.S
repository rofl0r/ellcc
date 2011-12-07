#define _LOCORE
#include <machine/asm.h>
#include <machine/cpuregs.h>

#define code "ax", @progbits

        .set mips32r2

/** The initial entry point.
 */
        .section .reset, code
        .set    noreorder
        .ent    _reset
_reset:
        la      k0, _start
        jr      k0
        nop
        .end    _reset
        .globl  _reset

        .section .reset.startup, code
        .set    noreorder
        .ent    _start
_start:
        /** The COP 0 status register indicates whether this is a
         * non-maskable interrupt or a reset.
         */
        mfc0    k0, MIPS_COP_0_STATUS
        ext     k0, k0, MIPS3_SR_NMI_BIT, 1
        beqz    k0, reset               // Jump if not NMI.
        nop
        la      k0, _nmi_handler        // Vector to the NMI handler.
        jr      k0
        nop

reset:
        /** The _stack and _gp symbols are set in the linker command file.
         * _stack is the top of the kernel stack area.
         * _gp points to the middle of the global data area.
         */
        la      sp, _stack              // Set up the stack pointer.
        la      gp, _gp                 // Initialize the global pointer.

        /** Copy the global pointer to all the other shadow register sets.
         */
        mfc0    t0, MIPS_COP_0_SRSCTL   // Get the shadow set control register.
        // Extract the highest shadow register set number.
        ext     t1, t0, MIPS_SRSC_HSS_BIT, MIPS_SRSC_HSS_SIZE
        beqz    t1, 2f                  // Jump if no shadow registers.
        nop
        move    t2, t0                  // Save the control register for later.
        // Set the previous shadow set number.
1:      ins     t0, t1, MIPS_SRSC_PSS_BIT, MIPS_SRSC_PSS_SIZE
        mtc0    t0, MIPS_COP_0_SRSCTL
        wrpgpr  gp, gp                  // Set the gp register in the previous set.
        addi    t1, -1                  // Decrement the register set number.
        bnez    t1, 1b                  // And do all the sets.
        nop
        mtc0    t2, MIPS_COP_0_SRSCTL   // Restore the original contents.
2:

        la      t0, _on_reset           // Call the on reset function.
        jalr    t0
        nop

        /** Initialize coprocessor 0.
         */

        /** The count and compare registers can be used together to
         * cause an interrupt when the count is equal to the compare
         * register.
         */
        mtc0    zero, MIPS_COP_0_COUNT  // Initialize the count register.
        li      t0, -1
        mtc0    t0, MIPS_COP_0_COMPARE  // Initialize the compare register.

        /** Set up exception vector handling.
         */
        la      t0, _ebase_address
        mtc0    t0, MIPS_COP_0_EBASE    // Set the vector base address.
        la      t0, _vector_spacing     // Get the specified vector spacing.
        move    t1, zero
        ins     t1, t0, MIPS_INTC_VS_BIT, MIPS_INTC_VS_SIZE
        mtc0    t1, MIPS_COP_0_INTCTL
        li      t0, MIPS_CAUSE_IV
        mtc0    t0, MIPS_COP_0_CAUSE

        /** Set up the status register.
         * Part of this code is pic32 specific: Moving the implementation defined
         * config UDI bit to the CEE bit of the status register.
         */
        mfc0    t0, MIPS_COP_0_CONFIG
        ext     t0, t0, 22, 1           // The UDI bit (User Defined Instructions).
        sll     t0, t0, 17              // Move to the pic32 CEE position.
        mfc0    t1, MIPS_COP_0_STATUS   // Get the status register.
        and     t1, t1, MIPS_SR_BEV|MIPS3_SR_SR|MIPS3_SR_NMI
        or      t1, t1, t0
        mtc0    t1, MIPS_COP_0_STATUS

        la      t0, _on_bootstrap       // Call the on bootstrap function.
        jalr    t0
        nop

        mfc0    t0, MIPS_COP_0_STATUS   // Get the status register.
        and     t0, t0, ~MIPS_SR_BEV    // Use the normal exception vectors.
        mtc0    t0, MIPS_COP_0_STATUS

        move    a0, zero                // argc
        move    a1, zero                // argv
        move    a2, zero                // environ

        /** Call _estart indirectly using a jal in the .text section so
         * the linker can change the jal to a jalx, if necessary,
         * depending on whether the target code is MIPS16 or MIPS32.
         */
        addi    sp, sp, -16             // Leave space for arguments 0..3 (o32 abi).
        la      t0,_estart_jmp
        jr      t0
        nop

        .end    _start
        .globl  _start

        .section .text, code
        .ent    _estart_jmp
_estart_jmp:
        jal     _estart
        nop
1:      b       1b                      // Enter an infinite loop if _estart returns.
        nop
        .end    _estart_jmp
