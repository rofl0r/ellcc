#include <sys/elf_note.h>

/** The initial entry point.
 */
        .text
        .align  16
        .globl  _start
        .type   _start,@function
_start:
        xorl    %ebp, %ebp              // Clear the link register.
        movl    (%esp), %eax            // Get argc...
        leal    4(%esp), %ecx           // ... and argv ...
        mov     %eax, %ebx              // ... copy argc ...
        inc     %ebx                    // ... argc + 1 ...
        leal    (%ecx, %ebx, 4), %edx   // ... and compute environ.
        subl    $12, %esp               // Room for arguments.
        movl    %edx, 8(%esp)
        movl    %ecx, 4(%esp)
        movl    %eax, (%esp)
        call    _estart                 // Let's go!
        jmp     .                       // Never gets here.
        .size   _start, .-_start
