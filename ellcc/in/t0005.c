// Multiple evaluation of conditional expressions.
 int
 main ()
 {
    int i;
    i = 0;
    ++i && ++i;
    if (i != 2) return 1;
    i = -1;
    ++i || ++i;
    if (i != 1) return 1;
    return 0;
 }
