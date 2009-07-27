/* realloc.c -- a wrapper for realloc_r.  */
#include <config.h>
#ifndef MALLOC_PROVIDED
#include <reent.h>
#include <stdlib.h>
#include <malloc.h>

void *realloc(void * ap, size_t nbytes)
{
  return _realloc_r (_REENT, ap, nbytes);
}
#endif /* MALLOC_PROVIDED */
