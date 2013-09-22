// RUN: %ppcecc -Werror -o %t %s
#include <wchar.h>
int main()
{
    wchar_t buffer[10];
    wcscmp(buffer, L"string");
}
