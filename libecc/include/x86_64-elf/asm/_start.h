/** The initial entry point.
 */
asm(
"       .text\n"
"       .align  16\n"
"       .globl  _start\n"
"       .type   _start,@function\n"
"_start:\n"
"       xor     %rbp, %rbp\n"           // Clear the link register.
"       mov     (%rsp), %rax\n"         // Get argc...
"       lea     4(%rsp), %rcx\n"        // ... and argv ...
"       lea     (%rcx, %rax, 4), %rdx\n"// ... and compute environ.
"       sub     $12, %rsp\n"            // Room for arguments.
"       mov     %rdx, 8(%rsp)\n"
"       mov     %rcx, 4(%rsp)\n"
"       mov     %rax, (%rsp)\n"
"       call    _estart\n"              // Let's go!
"       jmp     .\n"                    // Never gets here.
"       .size   _start, .-_start\n"
"       .type   _argc,@object\n"
);

