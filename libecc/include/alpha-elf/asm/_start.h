/** The initial entry point.
 */
asm(
"       .text\n"
"       .align  3\n"
"       .globl  _start\n"
"       .ent    _start, 0\n"
"       .type   _start,@function\n"
"_start:\n"
"       .frame  $15, 0, $15\n"
"       br      $29, 1f\n"
"1:     ldgp    $29, 0($29)\n"
"       subq    $30, 16, $30\n"
"       mov     0, $15\n"
"       .prologue 0\n"
"       ldl     $16, 16($30)\n"         // Get argc ...
"       lda     $17, 24($30)\n"         // ... and argv.
// RICH: Calculate environ.
"       lda     $27,_estart\n"
"       jsr     $26,($27),0\n"
"       br      .\n"                    // Never gets here.
"       .end     _start\n"
);
