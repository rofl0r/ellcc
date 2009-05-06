/* connector for read */

#include <reent.h>
#include <unistd.h>

int read(int fd, void *buf, size_t cnt)
{
  return _read_r(_REENT, fd, buf, cnt);
}
