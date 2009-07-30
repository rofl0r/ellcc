int f(int i);
int main()
{
    int (*g)();
    if (g == f || f == g) {
        g(1);
    }
}
