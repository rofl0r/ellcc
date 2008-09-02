#include <stdarg.h>

void g(void)
{
}

void f(/* int a, int b, int c, int d, */ char* p, ...)
{
    int i;
    va_list ap;
    va_start(ap, p);
    i = va_arg(ap, int);
    ++i;
    i = va_arg(ap, int);
    g();
    va_end(ap);
}

int main(void)
{
    char* p;
    int i;
    f(/* i, i, i, i, */ p, i);
}
