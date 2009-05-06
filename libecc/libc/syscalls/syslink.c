/* connector for link */

#include <reent.h>

int link(char *old, char *new)
{
  return _link_r(_REENT, old, new);
}
