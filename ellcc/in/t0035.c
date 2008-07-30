// Struct assignment through a pointer.
typedef struct {
    unsigned char b[8];
} foo;

int main(foo* n)
{
   foo n_copy;
   n_copy = *n;
}


