int f(int i);
int main()
{
    int (*g)();
    if (g == f) {
        g(1);
    }
}
