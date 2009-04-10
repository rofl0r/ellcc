int _swilseek (int ptr)
{
 int res;
 int fh;
 asm ("mov r0, %2; mov r1, %3; swi %a1; mov %0, r0"
 : "=r" (res)
 : "i" (0x6b), "r" (fh), "r" (ptr)
 : "r0", "r1");
}


