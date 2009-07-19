/* malloc.h -- header file for memory routines.  */

#ifndef _MALLOC_H_
#define _MALLOC_H_

#include <_ansi.h>
_BEGIN_STD_C
#include <sys/reent.h>

#define __need_size_t
#include <stddef.h>

/* include any machine-specific extensions */
#include <machine/malloc.h>

/* This version of struct mallinfo must match the one in
   libc/stdlib/mallocr.c.  */

struct mallinfo {
  int arena;    /* total space allocated from system */
  int ordblks;  /* number of non-inuse chunks */
  int smblks;   /* unused -- always zero */
  int hblks;    /* number of mmapped regions */
  int hblkhd;   /* total space in mmapped regions */
  int usmblks;  /* unused -- always zero */
  int fsmblks;  /* unused -- always zero */
  int uordblks; /* total allocated space */
  int fordblks; /* total non-inuse space */
  int keepcost; /* top-most, releasable (via malloc_trim) space */
};	

/* The routines.  */

extern void *malloc(size_t);
extern void * _malloc_r(struct _reent *, size_t);

extern void free(void *);
extern void _free_r(struct _reent *, void *);

extern void * realloc(void *, size_t);
extern void * _realloc_r(struct _reent *, void *, size_t);

extern void * calloc(size_t, size_t);
extern void * _calloc_r(struct _reent *, size_t, size_t);

extern void * memalign(size_t, size_t);
extern void * _memalign_r(struct _reent *, size_t, size_t);

extern struct mallinfo mallinfo(void);
extern struct mallinfo _mallinfo_r(struct _reent *);

extern void malloc_stats(void);
extern void _malloc_stats_r(struct _reent *);

extern int mallopt(int, int);
extern int _mallopt_r(struct _reent *, int, int);

extern size_t malloc_usable_size(void *);
extern size_t _malloc_usable_size_r(struct _reent *, void *);

/* These aren't too useful on an embedded system, but we define them
   anyhow.  */

extern void * valloc(size_t);
extern void * _valloc_r(struct _reent *, size_t);

extern void * pvalloc(size_t);
extern void * _pvalloc_r(struct _reent *, size_t);

extern int malloc_trim(size_t);
extern int _malloc_trim_r(struct _reent *, size_t);

/* A compatibility routine for an earlier version of the allocator.  */

extern void mstats(char *);
extern void _mstats_r(struct _reent *, char *);

/* SVID2/XPG mallopt options */

#define M_MXFAST  1    /* UNUSED in this malloc */
#define M_NLBLKS  2    /* UNUSED in this malloc */
#define M_GRAIN   3    /* UNUSED in this malloc */
#define M_KEEP    4    /* UNUSED in this malloc */

/* mallopt options that actually do something */
  
#define M_TRIM_THRESHOLD    -1
#define M_TOP_PAD           -2
#define M_MMAP_THRESHOLD    -3 
#define M_MMAP_MAX          -4

/* Some systems provide this, so do too for compatibility.  */
extern void cfree (void *);

_END_STD_C

#endif
