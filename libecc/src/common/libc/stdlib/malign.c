/* malign.c -- a wrapper for memalign_r.  */

#include <config.h>
#ifndef MALLOC_PROVIDED
#include <reent.h>
#include <stdlib.h>
#include <malloc.h>

void *memalign(size_t align, size_t nbytes)
{
  return _memalign_r (_REENT, align, nbytes);
}

#endif
