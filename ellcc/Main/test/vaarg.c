#include <stdarg.h>

void f(int a, int b, int c, char* p, ...)
{
    int i;
    va_list ap;
    va_start(ap, p);
    i = va_arg(ap, int);
    ++i;
    va_end(ap);
}

int main(void)
{
    char* p;
    int i;
    f(i, i, i, p, i);
}
