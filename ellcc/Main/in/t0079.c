struct info
{
    char c;
};

static const struct info ar[1] =
{
};

void
d_substitution ()
{
    &ar[0] + 1;
    ar[0] + 1;
}
