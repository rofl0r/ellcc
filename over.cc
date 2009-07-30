class A {
    int f(int);
    float f(float);
    int f;
};
int A::f(int);
float A::f(double);
int A::f;
int f(int);
float f(float);
int f;
