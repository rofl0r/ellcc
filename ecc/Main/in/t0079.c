struct info
{
    char c;
};

static struct info ar[1] =
{
};

void main()
{
    &ar[0] + 1;
    struct info b;
    b+1;
    //ERROR(1): ar[0] + 1;
}
