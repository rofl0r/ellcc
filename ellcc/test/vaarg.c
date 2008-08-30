#include <stdarg.h>
void f(char* p, ...)
{
    va_list ap;
    va_start(ap, p);
    va_end(ap);
}

int main(void)
{
    char* p;
    int i;
    f(p, i);
}
