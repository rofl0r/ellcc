void foo ()
{
   int rr;
   asm ("nop" : "=r"(rr) : "0"(rr));
}
