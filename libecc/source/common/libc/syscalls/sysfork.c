/* connector for fork */

#include <reent.h>

int fork(void)
{
  return _fork_r(_REENT);
}
