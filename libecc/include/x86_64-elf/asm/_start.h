/** The initial entry point.
 */
asm(
"       .text\n"
"       .globl  _start\n"
"       .align  2\n"
"_start:\n"
"       .text\n"
"       .align  16\n"
"       .globl  _start\n"
"       .type   _start,@function\n"
"_start:\n"
"       xorl    %rbp, %rbp\n"           // Clear the link register.
"       movl    (%rsp), %rax\n"         // Get argc...
"       leal    4(%rsp), %rcx\n"        // ... and argv ...
"       leal    (%rcx, %rax, 4), %rdx\n"// ... and compute environ.
"       subl    $12, %rsp\n"            // Room for arguments.
"       movl    %rdx, 8(%rsp)\n"
"       movl    %rcx, 4(%rsp)\n"
"       movl    %rax, (%rsp)\n"
"       call    _estart\n"              // Let's go!
"       jmp     .\n"                    // Never gets here.
"       .size   _start, .-_start\n"
"       .type   _argc,@object\n"
);

