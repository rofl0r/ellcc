    class F;
    class E {
    public:
        int f;
        E();
        E(F&);
    }ee;
    class F {
    public:
        int f;
        F();
        F(E&);
    }ff;

    class G {
    public:
        int f;
    }gg;

int main()
{
    1 ? ee : ff;

    1 ? gg : ff;
    sizeof(struct { int a; } *);
    typedef int abc;
    abc = 5;
}

struct A {
  operator int* ();
  operator float* ();
  operator void* ();
};

struct B {
};
void f()
{
  A a;
  delete a;
  B b;
  delete b;
  dynamic_cast<struct { int a; } *>(v);
}
  template<typename T, typename T> class C { T v; } a, i;
