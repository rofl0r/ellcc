#include <stdio.h>
double __adddf3(double a, double b);
int main(int argc, char** argv)
{
    double u = 1.0; // atof(argv[1]);
    double v = 1.0; // atof(argv[2]);
    // double c = u + v;
    double c = __adddf3(u, v);
    printf("c = %f\n", c);
}
