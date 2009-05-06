/* connector for wait */

#include <reent.h>

int wait(int *status)
{
  return _wait_r(_REENT, status);
}
