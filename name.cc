namespace foo {
    int i;
    template<typename T> F f() {}
};

namespace fie {
    int i;
};

int main()
{
    namespace f = foe;
    namespace f = foo;
    f::i=1;
    namespace f = fie;
    using foo::f<int>;
    g();
}
struct {
} s;
