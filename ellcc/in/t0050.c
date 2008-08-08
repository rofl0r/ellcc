#if 1
void print_number();
#else
void print_number(int base, int unsigned_p, long n);
#endif

void __dprintf (char *fmt, ...)
{
    char* p;
    char c;
    long l;

    print_number (c /* == 'x' ? 16 : 10 */, c /* != 'd' */, l);
}

void print_number (base, unsigned_p, n)
 int base;
 int unsigned_p;
 long n;
{
}

int main()
{
}
