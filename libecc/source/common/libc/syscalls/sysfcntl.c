/* connector for fcntl */
/* only called from stdio/fdopen.c, so arg can be int. */

#include <reent.h>
#include <errno.h>

int fcntl(int fd, int flag, int arg)
{
  return _fcntl_r (_REENT, fd, flag, arg);
}
