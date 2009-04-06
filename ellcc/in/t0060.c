float _f_expf (float x)
{
 float result;
 asm ("fldl2e; fmulp; fld %%st; frndint; fsub %%st,%%st(1); fxch;" "fchs; f2xm1; fld1; faddp; fxch; fld1; fscale; fstp %%st(1); fmulp"
 : "=t"(result) : "0"(x));
 return result;
}
