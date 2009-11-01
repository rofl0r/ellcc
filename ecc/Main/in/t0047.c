int f()
{
}

int main()
{
    int (*fp)();

    fp = 1 ? f : 0;
}
