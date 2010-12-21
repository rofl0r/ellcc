/*
 * gmtime_r.c
 */

#include <time.h>
#include "local.h"

struct tm *gmtime_r(const time_t * tim_p, struct tm *res)
{
  return _mktm_r(tim_p, res, 1);
}
