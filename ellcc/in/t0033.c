// Predeclaration of a function with an empty argument list.
int main ();

#if 0
int main (ptr, s)
    char* ptr;
    const char *s;
#else
int main (char* ptr, const char *s)
#endif
{
}
