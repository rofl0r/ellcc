/** The initial entry point.
 */
        .text
        .align  16
        .globl  _start
        .type   _start,@function
_start:
        xor     %rbp, %rbp              // Clear the link register.
        mov     (%rsp), %rdi            // Get argc...
        lea     8(%rsp), %rsi           // ... and argv ...
        mov     %rax, %rbx              // ... copy argc ...
        inc     %rbx                    // ... argc + 1 ...
        lea     (%rsi, %rbx, 8), %rdx   // ... and compute environ.
        andq    $~15, %rsp              // Align the stack on a 16 byte boundry.
        call    _estart                 // Let's go!
        jmp     .                       // Never gets here.
        .size   _start, .-_start

