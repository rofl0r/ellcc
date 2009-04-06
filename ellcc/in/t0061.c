long long int _f_llrint (double x)
{
 long long int _result;
 asm ("fistpll %0" : "=m" (_result) : "t" (x) : "st");
 return _result;
}
