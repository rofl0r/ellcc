/* default reentrant pointer when multithread enabled */

#include <reent.h>

#ifdef __getreent
#undef __getreent
#endif

struct _reent *__getreent(void)
{
  return _impure_ptr;
}
