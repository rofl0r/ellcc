/*
 * Copyright (c) 1990, 2007 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	%W% (UofMD/Berkeley) %G%
 */

/*
 * Information local to this implementation of stdio,
 * in particular, macros and private variables.
 */

#include <_ansi.h>
#include <reent.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#ifdef __SCLE
# include <io.h>
#endif


extern int    __svfscanf_r(struct _reent *,FILE *, const char *,va_list);
extern int    __ssvfscanf_r(struct _reent *,FILE *, const char *,va_list);
extern int    __svfiscanf_r(struct _reent *,FILE *, const char *,va_list);
extern int    __ssvfiscanf_r(struct _reent *,FILE *, const char *,va_list);
int	      _svfprintf_r(struct _reent *, FILE *, const char *, va_list)
               			__attribute__ ((__format__ (__printf__, 3, 0)));
int	      _svfiprintf_r(struct _reent *, FILE *, const char *, 
				  va_list)
               			__attribute__ ((__format__ (__printf__, 3, 0)));
extern FILE  *__sfp(struct _reent *);
extern int    __sflags(struct _reent *,const char*, int*);
extern int    __srefill_r(struct _reent *,FILE *);
extern int __sread(struct _reent *, void *, char *,
					       int);
extern int __swrite(struct _reent *, void *,
						const char *, int);
extern _fpos_t __sseek(struct _reent *, void *, _fpos_t, int);
extern int    __sclose(struct _reent *, void *);
extern int    __stextmode(int);
extern void   __sinit(struct _reent *);
extern void   _cleanup_r(struct _reent *);
extern void   __smakebuf_r(struct _reent *, FILE *);
extern int    _fwalk(struct _reent *, int (*)(FILE *));
extern int    _fwalk_reent(struct _reent *, int (*)(struct _reent *, FILE *));
struct _glue * __sfmoreglue(struct _reent *,int n);

#ifdef __LARGE64_FILES
extern _fpos64_t __sseek64(struct _reent *, void *, _fpos64_t, int);
extern int __swrite64(struct _reent *, void *,
						  const char *, int);
#endif

/* Called by the main entry point fns to ensure stdio has been initialized.  */

#ifdef _REENT_SMALL
#define CHECK_INIT(ptr, fp) \
  do						\
    {						\
      if ((ptr) && !(ptr)->__sdidinit)		\
	__sinit (ptr);				\
      if ((fp) == (FILE *)&__sf_fake_stdin)	\
	(fp) = _stdin_r(ptr);			\
      else if ((fp) == (FILE *)&__sf_fake_stdout) \
	(fp) = _stdout_r(ptr);			\
      else if ((fp) == (FILE *)&__sf_fake_stderr) \
	(fp) = _stderr_r(ptr);			\
    }						\
  while (0)
#else /* !_REENT_SMALL   */
#define CHECK_INIT(ptr, fp) \
  do						\
    {						\
      if ((ptr) && !(ptr)->__sdidinit)		\
	__sinit (ptr);				\
    }						\
  while (0)
#endif /* !_REENT_SMALL  */

#define CHECK_STD_INIT(ptr) \
  do						\
    {						\
      if ((ptr) && !(ptr)->__sdidinit)		\
	__sinit (ptr);				\
    }						\
  while (0)

/* Return true iff the given FILE cannot be written now.  */

#define	cantwrite(ptr, fp)                                     \
  ((((fp)->_flags & __SWR) == 0 || (fp)->_bf._base == NULL) && \
   __swsetup_r(ptr, fp))

/* Test whether the given stdio file has an active ungetc buffer;
   release such a buffer, without restoring ordinary unread data.  */

#define	HASUB(fp) ((fp)->_ub._base != NULL)
#define	FREEUB(ptr, fp) {                    \
	if ((fp)->_ub._base != (fp)->_ubuf) \
		_free_r(ptr, (char *)(fp)->_ub._base); \
	(fp)->_ub._base = NULL; \
}

/* Test for an fgetline() buffer.  */

#define	HASLB(fp) ((fp)->_lb._base != NULL)
#define	FREELB(ptr, fp) { _free_r(ptr, (char *)(fp)->_lb._base); \
      (fp)->_lb._base = NULL; }

/* WARNING: _dcvt is defined in the stdlib directory, not here!  */

char *_dcvt(struct _reent *, char *, double, int, int, char, int);
char *_sicvt(char *, short, char);
char *_icvt(char *, int, char);
char *_licvt(char *, long, char);
#ifdef __GNUC__
char *_llicvt(char *, long long, char);
#endif

#define CVT_BUF_SIZE 128

#define	NDYNAMIC 4	/* add four more whenever necessary */

#ifdef __SINGLE_THREAD__
#define __sfp_lock_acquire()
#define __sfp_lock_release()
#define __sinit_lock_acquire()
#define __sinit_lock_release()
#else
void __sfp_lock_acquire(void);
void __sfp_lock_release(void);
void __sinit_lock_acquire(void);
void __sinit_lock_release(void);
#endif
