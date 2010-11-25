/* msize.c -- a wrapper for malloc_usable_size.  */

#include <config.h>
#ifndef MALLOC_PROVIDED
#include <reent.h>
#include <stdlib.h>
#include <malloc.h>

size_t malloc_usable_size(void * ptr)
{
  return _malloc_usable_size_r (_REENT, ptr);
}
#endif
