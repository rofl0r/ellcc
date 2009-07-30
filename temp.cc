template<typename T> T f(T a) { return a; }

extern int printf(const char*, ...);
int main()
{
    printf("f<int> = %d, f<double> %g f<long long> %llX\n",
        f(10),
        f(3.1415927),
        f<long long>(0x1234567890123456LL));
}

template<typename T> class C { int a; int f<T>(); } x;

template <class T, class U>
int f();

void g()
{
  //ERROR(1): f<int>();
  f<int,int>();
  // f<int,int,int>();
}

