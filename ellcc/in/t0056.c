#include <stdarg.h>

int main(va_list ap)
{
    typedef float type;
    type tmp;

    tmp = va_arg(ap, type);
}

