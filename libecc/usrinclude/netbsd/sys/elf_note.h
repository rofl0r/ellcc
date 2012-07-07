#include <sys/param.h>                  // For __NetBSD_Version__.

#define xstr(x) #x
#define str(x) xstr(x)
asm(
"       .section \".note.netbsd.ident\", \"a\"\n"
"       .align 4\n"
"       .long   1f - 0f\n"              // Size of the name "NetBSD".
"       .long   3f - 2f\n"              // Size of the description (OS version number)
"       .long   1\n"                    // NT_VERSION
"0:     .asciz  \"NetBSD\"\n"
"1:     .align 4\n"
"2:     .long   " str(__NetBSD_Version__) "\n"            
"3:     .align 4\n"

"       .section \".note.netbsd.pax\", \"a\"\n"
"       .align 4\n"
"       .long   1f - 0f\n"              // Size of the name "NetBSD".
"       .long   3f - 2f\n"              // Size of the description (OS version number)
"       .long   3\n"                    // Type.
"0:     .asciz  \"PaX\"\n"
"1:     .align 4\n"
"2:     .long  0\n"            
"3:     .align 4\n"
);
