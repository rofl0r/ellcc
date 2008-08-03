struct foo {
    int i1;
    int i2;
    int i3;
};

void func(struct foo f)
{
}

struct foo f;

int main()
{
    func(f);
}
