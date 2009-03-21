register char * stack_ptr asm ("sp");

//  asm("nop");
void initialise_monitor_handles (void)
{
 int i;
 int fh;
 const char * name;

 name = ":tt";
 asm ("mov r0,%2; mov r1, #0; swi %a1; mov %0, r0"
 : "=r"(fh)
 : "i" (0x66),"r"(name)
 : "r0","r1");

 name = ":tt";
 asm ("mov r0,%2; mov r1, #4; swi %a1; mov %0, r0"
 : "=r"(fh)
 : "i" (0x66),"r"(name)
 : "r0","r1");
}

static int get_errno (void)
{
 asm ("swi %a0" :: "i" (0x60));
}

int _swiread (char * ptr, int len)
{
 int fh;

 asm ("mov r0, %1; mov r1, %2;mov r2, %3; swi %a0"
 :  
 : "i"(0x6a), "r"(fh), "r"(ptr), "r"(len)
 : "r0","r1","r2");

}

int _swilseek (int ptr)
{
 int res;
 int fh;
 asm ("mov r0, %2; swi %a1; mov %0, r0"
 : "=r" (res)
 : "i" (0x6c), "r" (fh)
 : "r0");
 ptr += res;
  
 asm ("mov r0, %2; mov r1, %3; swi %a1; mov %0, r0"
 : "=r" (res)
 : "i" (0x6b), "r" (fh), "r" (ptr)
 : "r0", "r1");

 return res == 0 ? ptr : -1;
}

int _swiwrite ( char * ptr, int len)
{
 int fh;

 asm ("mov r0, %1; mov r1, %2;mov r2, %3; swi %a0"
 :  
 : "i"(0x69), "r"(fh), "r"(ptr), "r"(len)
 : "r0","r1","r2");
}

int _swiopen (char* path)
{
 int aflags = 0, fh;
 int i;
 
 asm ("mov r0,%2; mov r1, %3; swi %a1; mov %0, r0"
 : "=r"(fh)
 : "i" (0x66),"r"(path),"r"(aflags)
 : "r0","r1");
 return fh;
}

int _swiclose ()
{
 int myhan;
 
 asm ("mov r0, %1; swi %a0" :: "i" (0x68),"r"(myhan):"r0");
}

int _kill (int pid, int sig)
{
 (void)pid; (void)sig;
 asm ("swi %a0" :: "i" (0x11));
}

int _gettimeofday ()
{
 int value;
 asm ("swi %a1; mov %0, r0" : "=r" (value): "i" (0x63) : "r0");
 return 0;
}

 
int _times ()
{
 int timeval;
 asm ("swi %a1; mov %0, r0" : "=r" (timeval): "i" (0x61) : "r0");
 return timeval;
};
