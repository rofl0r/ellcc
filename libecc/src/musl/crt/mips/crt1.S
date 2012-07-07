#include <bits/asm.h>
        .weak  _init
        .weak  _fini
        .global _start
        .align  2
        .ent    _start
_start:
        .mask   0x00000000,0
        .fmask  0x00000000,0
        subu    fp, fp, fp              // Zero the frame pointer.
        la      gp, _gp
        lw      a0, %call16(main)(gp)   // Get main() ...
        lw      a1, (sp)                // Get argc...
        addu    a2, sp, 4               // and argv ...
        lw      a3, %call16(_init)(gp)  // and _init() ...
#if defined(__mips_n64)
#error The 64 bit Mips processor is not supported.
#elif defined(__mips_n32)
        lw      a4, %call16(_fini)(gp)  // and _fini() ...
        move    a5, zero                // ldso_fini == NULL
#else /* __mips_o32 */
        addi    sp, sp, -4*6            // Leave space for arguments 0..3, arg4, and arg5.
        lw      t0, %call16(_fini)(gp)  // and _fini() ...
        sw      t0, 4*4(sp)             // Save arg4.
        sw      zero, 4*5(sp)           // ldso_fini == NULL
#endif
        lw      t0, %call16(__libc_start_main)(gp)
        jalr    t0                      // Let's go!
        nop
        b       .                       // Never gets here.
        nop
        .end    _start
        .size   _start, .-_start
