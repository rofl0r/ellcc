/* connector for gettimeofday */

#include <reent.h>
#include <sys/types.h>
#include <sys/times.h>

struct timeval;
struct timezone;

int gettimeofday(struct timeval *ptimeval, struct timezone *ptimezone)
{
  return _gettimeofday_r(_REENT, ptimeval, ptimezone);
}
