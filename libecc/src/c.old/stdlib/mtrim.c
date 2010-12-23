/* mtrim.c -- a wrapper for malloc_trim.  */
#include <config.h>
#ifndef MALLOC_PROVIDED
#include <reent.h>
#include <stdlib.h>
#include <malloc.h>

int malloc_trim(size_t pad)
{
  return _malloc_trim_r(_REENT, pad);
}
#endif
