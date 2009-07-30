// int main() asm ("g");
int main() asm ("f");
int i asm ("x");
// int i asm ("y");

    // register int b asm("ex");
int j asm ("k") = 2;
int main()
{
    register int i asm("ex");
    register int b asm("eax");
    i = 0;
    b = i;
  //   struct { int a; } b asm("c");
}
