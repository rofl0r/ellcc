typedef struct
{
 int i;
 int j;
} foo;

int main()
{
    foo *p;
    *p = (foo){0, 0};
}
