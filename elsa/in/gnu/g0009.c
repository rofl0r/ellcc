// g0009.c
// test __FUNCTION__ etc. in C mode

void f()
{
  char const *a = __func__;
  char *b = __FUNCTION__;
  char *c = __PRETTY_FUNCTION__;

  //ERROR(1):   char *d = something_else;             // undefined

  //ERROR(2):   char *d = "x" __func__;               // no concat
}
