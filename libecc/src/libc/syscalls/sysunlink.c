/* connector for unlink */

#include <reent.h>

int unlink(char *file)
{
  return _unlink_r(_REENT, file);
}
