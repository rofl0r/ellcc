#define OPS(t1, t2)                             \
{                                               \
    t1 o1;                                      \
    t2 o2;                                      \
    o1 += o2;                                   \
    o1 -= o2;                                   \
    o1 *= o2;                                   \
    o1 /= o2;                                   \
    o1 %= o2;                                   \
    o1 &= o2;                                   \
    o1 != o2;                                   \
    o1 ^= o2;                                   \
    o1 <<= o2;                                  \
    o1 >>= o2;                                  \
}

int main()
{
    OPS(long, long)
    OPS(unsigned long, unsigned long)
    OPS(long, unsigned long)
    OPS(unsigned long, long)
    OPS(long long, long)
}
