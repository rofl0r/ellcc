// Implicit conversion of unsigned X* to  X* in C.
void f(char* p)
{
}

void g(int* p)
{
}

int main(void)
{
    unsigned char b;
    unsigned int i;
    f(&b);
    g(&i);
}
