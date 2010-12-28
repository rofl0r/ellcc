/** The initial entry point.
 */
asm(
"       .text\n"
"       .align  16\n"
"       .globl  _start\n"
"       .type   _start,@function\n"
"_start:\n"
"       xor     %rbp, %rbp\n"           // Clear the link register.
"       mov     (%rsp), %rdi\n"         // Get argc...
"       lea     8(%rsp), %rsi\n"        // ... and argv ...
"       mov     %rax, %rbx\n"           // ... copy argc ...
"       inc     %rbx\n"                 // ... argc + 1 ...
"       lea     (%rsi, %rbx, 8), %rdx\n"// ... and compute environ.
"       andq    $~15, %rsp\n"           // Align the stack on a 16 byte boundry.
"       call    _estart\n"              // Let's go!
"       jmp     .\n"                    // Never gets here.
"       .size   _start, .-_start\n"
"       .type   _argc,@object\n"
);

