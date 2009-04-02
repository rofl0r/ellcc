double _f_exp (double x)
{
 double result;
 asm ("fldl2e; fmulp; fld %%st; frndint; fsub %%st,%%st(1); fxch;"
      "fchs; f2xm1; fld1; faddp; fxch; fld1; fscale; fstp %%st(1); fmulp"
      : [res] "=t"(result)
      : "[res]"(x));
 return result;
}
