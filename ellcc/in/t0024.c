// Function pointer in a struct.
struct foo {
    void (*f)(void);
};

int main()
{
    struct foo* f;
    f->f();
}
