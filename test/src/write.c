#include <unistd.h>
int main(int argc, char** argv)
{
    write(1, "hello world\n", sizeof("hello world\n") - 1);
}
