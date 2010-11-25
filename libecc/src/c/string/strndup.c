#include <reent.h>
#include <stdlib.h>
#include <string.h>

char *strndup(const char *str, size_t n)
{
  return _strndup_r (_REENT, str, n);
}
