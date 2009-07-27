/*
 * Andy Wilson, 2-Oct-89.
 */

#include <stdlib.h>

long atol(const char *s)
{
  return strtol (s, NULL, 10);
}

long _atol_r(struct _reent *ptr, const char *s)
{
  return _strtol_r (ptr, s, NULL, 10);
}
