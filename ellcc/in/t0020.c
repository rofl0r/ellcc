// Array address in assignment.
int main()
{
    short ar[1];
#if 1
    short *equot = ar;
#else
    short *equot;
    equot = ar;
#endif
}
