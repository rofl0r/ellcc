/** The initial entry point.
 */
asm(
"       .text\n"
"       .align  16\n"
"       .globl  _start\n"
"       .type   _start,@function\n"
"_start:\n"
"       xorl    %ebp, %ebp\n"           // Clear the link register.
"       movl    (%esp), %eax\n"         // Get argc...
"       leal    4(%esp), %ecx\n"        // ... and argv ...
"       mov     %eax, %ebx\n"           // ... copy argc ...
"       inc     %ebx\n"                 // ... argc + 1 ...
"       leal    (%ecx, %ebx, 4), %edx\n"// ... and compute environ.
"       subl    $12, %esp\n"            // Room for arguments.
"       movl    %edx, 8(%esp)\n"
"       movl    %ecx, 4(%esp)\n"
"       movl    %eax, (%esp)\n"
"       call    _estart\n"              // Let's go!
"       jmp     .\n"                    // Never gets here.
"       .size   _start, .-_start\n"
);

