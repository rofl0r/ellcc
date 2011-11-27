#include <sys/elf_note.h>

/** The initial entry point.
 */
        .text
        .globl  _start
        .type   _start, %function
_start:
        mr      9, 1                    // Save the original stack pointer.
        clrrwi  1, 1, 4                 // Align the stack to 16 bytes.
        lis     13, _SDA_BASE_@ha       // r13 points to the small data area.
        addi    13, 13, _SDA_BASE_@l    //
        li      0, 0                    // Clear the frame pointer.
        stwu    1, -16(1)               // The initial stack frame.
        mtlr    0                       // Clear the link register.
        stw     0, 0(1)                 // And save it.
        lwz     3, 0(9)                 // Get argc...
        addi    4, 9, 4                 // ... and argv ...
        addi    5, 3, 1                 // ... and argc + 1 ...
        add     5, 5, 5                 // ... (argc + 1) * 2
        add     5, 5, 5                 // ... (argc + 1) * 4 to compute environ.
        add     5, 5, 4                 // ... indexed from argv.
        b       _estart                 // Let's go!
        .size   _start, .-_start

