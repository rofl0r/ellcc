/*
 * Copyright (c) 1990 The Regents of the University of California.
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
 *	@(#)stdio.h	5.3 (Berkeley) 3/15/86
 */

/*
 * NB: to fit things in six character monocase externals, the
 * stdio code uses the prefix `__s' for stdio objects, typically
 * followed by a three-character attempt at a mnemonic.
 */

#ifndef _STDIO_H_
#define	_STDIO_H_

#include "_ansi.h"
_BEGIN_STD_C

#define	_FSTDIO			/* ``function stdio'' */

#define __need_size_t
#include <stddef.h>

#define __need___va_list
#include <stdarg.h>

/*
 * <sys/reent.h> defines __FILE, _fpos_t.
 * They must be defined there because struct _reent needs them (and we don't
 * want reent.h to include this file.
 */

#include <sys/reent.h>
#include <sys/types.h>

typedef __FILE FILE;

typedef _fpos_t fpos_t;
#ifdef __LARGE64_FILES
typedef _fpos64_t fpos64_t;
#endif

#include <sys/stdio.h>

#define	__SLBF	0x0001		/* line buffered */
#define	__SNBF	0x0002		/* unbuffered */
#define	__SRD	0x0004		/* OK to read */
#define	__SWR	0x0008		/* OK to write */
	/* RD and WR are never simultaneously asserted */
#define	__SRW	0x0010		/* open for reading & writing */
#define	__SEOF	0x0020		/* found EOF */
#define	__SERR	0x0040		/* found error */
#define	__SMBF	0x0080		/* _buf is from malloc */
#define	__SAPP	0x0100		/* fdopen()ed in append mode - so must  write to end */
#define	__SSTR	0x0200		/* this is an sprintf/snprintf string */
#define	__SOPT	0x0400		/* do fseek() optimisation */
#define	__SNPT	0x0800		/* do not do fseek() optimisation */
#define	__SOFF	0x1000		/* set iff _offset is in fact correct */
#define	__SMOD	0x2000		/* true => fgetline modified _p text */
#define	__SL64	0x8000		/* is 64-bit offset large file */

/*
 * The following three definitions are for ANSI C, which took them
 * from System V, which stupidly took internal interface macros and
 * made them official arguments to setvbuf(), without renaming them.
 * Hence, these ugly _IOxxx names are *supposed* to appear in user code.
 *
 * Although these happen to match their counterparts above, the
 * implementation does not rely on that (so these could be renumbered).
 */
#define	_IOFBF	0		/* setvbuf should set fully buffered */
#define	_IOLBF	1		/* setvbuf should set line buffered */
#define	_IONBF	2		/* setvbuf should set unbuffered */

#ifndef NULL
#define	NULL	0
#endif

#define	EOF	(-1)

#ifdef __BUFSIZ__
#define	BUFSIZ		__BUFSIZ__
#else
#define	BUFSIZ		1024
#endif

#ifdef __FOPEN_MAX__
#define FOPEN_MAX	__FOPEN_MAX__
#else
#define	FOPEN_MAX	20
#endif

#ifdef __FILENAME_MAX__
#define FILENAME_MAX    __FILENAME_MAX__
#else
#define	FILENAME_MAX	1024
#endif

#ifdef __L_tmpnam__
#define L_tmpnam	__L_tmpnam__
#else
#define	L_tmpnam	FILENAME_MAX
#endif

#ifndef __STRICT_ANSI__
#define P_tmpdir        "/tmp"
#endif

#ifndef SEEK_SET
#define	SEEK_SET	0	/* set file offset to offset */
#endif
#ifndef SEEK_CUR
#define	SEEK_CUR	1	/* set file offset to current plus offset */
#endif
#ifndef SEEK_END
#define	SEEK_END	2	/* set file offset to EOF plus offset */
#endif

#define	TMP_MAX		26

#ifndef _REENT_ONLY
#define	stdin	(_REENT->_stdin)
#define	stdout	(_REENT->_stdout)
#define	stderr	(_REENT->_stderr)
#else /* _REENT_ONLY */
#define	stdin	(_impure_ptr->_stdin)
#define	stdout	(_impure_ptr->_stdout)
#define	stderr	(_impure_ptr->_stderr)
#endif /* _REENT_ONLY */

#define _stdin_r(x)	((x)->_stdin)
#define _stdout_r(x)	((x)->_stdout)
#define _stderr_r(x)	((x)->_stderr)

/*
 * Functions defined in ANSI C standard.
 */

FILE *	tmpfile(void);
char *	tmpnam(char *);
int	fclose(FILE *);
int	fflush(FILE *);
FILE *	freopen(const char *, const char *, FILE *);
void	setbuf(FILE *, char *);
int	setvbuf(FILE *, char *, int, size_t);
int	fprintf(FILE *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 2, 3)));
int	fscanf(FILE *, const char *, ...)
               __attribute__ ((__format__ (__scanf__, 2, 3)));
int	printf(const char *, ...)
               __attribute__ ((__format__ (__printf__, 1, 2)));
int	scanf(const char *, ...)
               __attribute__ ((__format__ (__scanf__, 1, 2)));
int	sscanf(const char *, const char *, ...)
               __attribute__ ((__format__ (__scanf__, 2, 3)));
int	vfprintf(FILE *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 2, 0)));
int	vprintf(const char *, va_list)
               __attribute__ ((__format__ (__printf__, 1, 0)));
int	vsprintf(char *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 2, 0)));
int	fgetc(FILE *);
char *  fgets(char *, int, FILE *);
int	fputc(int, FILE *);
int	fputs(const char *, FILE *);
int	getc(FILE *);
int	getchar(void);
char *  gets(char *);
int	putc(int, FILE *);
int	putchar(int);
int	puts(const char *);
int	ungetc(int, FILE *);
size_t	fread(void *, size_t _size, size_t _n, FILE *);
size_t	fwrite(const void * , size_t _size, size_t _n, FILE *);
#ifdef _COMPILING_NEWLIB
int	fgetpos(FILE *, _fpos_t *);
#else
int	fgetpos(FILE *, fpos_t *);
#endif
int	fseek(FILE *, long, int);
#ifdef _COMPILING_NEWLIB
int	fsetpos(FILE *, const _fpos_t *);
#else
int	fsetpos(FILE *, const fpos_t *);
#endif
long	ftell( FILE *);
void	rewind(FILE *);
void	clearerr(FILE *);
int	feof(FILE *);
int	ferror(FILE *);
void    perror(const char *);
#ifndef _REENT_ONLY
FILE *	fopen(const char *_name, const char *_type);
int	sprintf(char *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 2, 3)));
int	remove(const char *);
int	rename(const char *, const char *);
#endif
#ifndef __STRICT_ANSI__
#ifdef _COMPILING_NEWLIB
int	fseeko(FILE *, _off_t, int);
_off_t	ftello( FILE *);
#else
int	fseeko(FILE *, off_t, int);
off_t	ftello( FILE *);
#endif
#ifndef _REENT_ONLY
int	asiprintf(char **, const char *, ...)
               __attribute__ ((__format__ (__printf__, 2, 3)));
char *	asniprintf(char *, size_t *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 3, 4)));
char *	asnprintf(char *, size_t *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 3, 4)));
int	asprintf(char **, const char *, ...)
               __attribute__ ((__format__ (__printf__, 2, 3)));
#ifndef diprintf
int	diprintf(int, const char *, ...)
               __attribute__ ((__format__ (__printf__, 2, 3)));
#endif
int	fcloseall(void);
int	fiprintf(FILE *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 2, 3)));
int	fiscanf(FILE *, const char *, ...)
               __attribute__ ((__format__ (__scanf__, 2, 3)));
int	iprintf(const char *, ...)
               __attribute__ ((__format__ (__printf__, 1, 2)));
int	iscanf(const char *, ...)
               __attribute__ ((__format__ (__scanf__, 1, 2)));
int	siprintf(char *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 2, 3)));
int	siscanf(const char *, const char *, ...)
               __attribute__ ((__format__ (__scanf__, 2, 3)));
int	snprintf(char *, size_t, const char *, ...)
               __attribute__ ((__format__ (__printf__, 3, 4)));
int	sniprintf(char *, size_t, const char *, ...)
               __attribute__ ((__format__ (__printf__, 3, 4)));
char *	tempnam(const char *, const char *);
int	vasiprintf(char **, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 2, 0)));
char *	vasniprintf(char *, size_t *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 3, 0)));
char *	vasnprintf(char *, size_t *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 3, 0)));
int	vasprintf(char **, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 2, 0)));
int	vdiprintf(int, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 2, 0)));
int	vfiprintf(FILE *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 2, 0)));
int	vfiscanf(FILE *, const char *, va_list)
               __attribute__ ((__format__ (__scanf__, 2, 0)));
int	vfscanf(FILE *, const char *, va_list)
               __attribute__ ((__format__ (__scanf__, 2, 0)));
int	viprintf(const char *, va_list)
               __attribute__ ((__format__ (__printf__, 1, 0)));
int	viscanf(const char *, va_list)
               __attribute__ ((__format__ (__scanf__, 1, 0)));
int	vscanf(const char *, va_list)
               __attribute__ ((__format__ (__scanf__, 1, 0)));
int	vsiprintf(char *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 2, 0)));
int	vsiscanf(const char *, const char *, va_list)
               __attribute__ ((__format__ (__scanf__, 2, 0)));
int	vsniprintf(char *, size_t, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 3, 0)));
int	vsnprintf(char *, size_t, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 3, 0)));
int	vsscanf(const char *, const char *, va_list)
               __attribute__ ((__format__ (__scanf__, 2, 0)));
#endif /* !_REENT_ONLY */
#endif /* !__STRICT_ANSI__ */

/*
 * Routines in POSIX 1003.1:2001.
 */

#ifndef __STRICT_ANSI__
#ifndef _REENT_ONLY
FILE *	fdopen(int, const char *);
#endif
int	fileno(FILE *);
int	getw(FILE *);
int	pclose(FILE *);
FILE *  popen(const char *, const char *);
int	putw(int, FILE *);
void    setbuffer(FILE *, char *, int);
int	setlinebuf(FILE *);
int	getc_unlocked(FILE *);
int	getchar_unlocked(void);
void	flockfile(FILE *);
int	ftrylockfile(FILE *);
void	funlockfile(FILE *);
int	putc_unlocked(int, FILE *);
int	putchar_unlocked(int);
#endif /* ! __STRICT_ANSI__ */

/*
 * Routines in POSIX 1003.1:200x.
 */

#ifndef __STRICT_ANSI__
# ifndef _REENT_ONLY
#  ifndef dprintf
int	dprintf(int, const char *, ...)
               __attribute__ ((__format__ (__printf__, 2, 3)));
#  endif
FILE *	fmemopen(void *, size_t, const char *);
/* getdelim - see __getdelim for now */
/* getline - see __getline for now */
FILE *	open_memstream(char **, size_t *);
#if defined (__CYGWIN__)
int	renameat(int, const char *, int, const char *);
int	symlinkat(const char *, int, const char *);
#endif
int	vdprintf(int, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 2, 0)));
# endif
#endif

/*
 * Recursive versions of the above.
 */

int	_asiprintf_r(struct _reent *, char **, const char *, ...)
               __attribute__ ((__format__ (__printf__, 3, 4)));
char *	_asniprintf_r(struct _reent *, char *, size_t *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 4, 5)));
char *	_asnprintf_r(struct _reent *, char *, size_t *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 4, 5)));
int	_asprintf_r(struct _reent *, char **, const char *, ...)
               __attribute__ ((__format__ (__printf__, 3, 4)));
int	_diprintf_r(struct _reent *, int, const char *, ...)
               __attribute__ ((__format__ (__printf__, 3, 4)));
int	_dprintf_r(struct _reent *, int, const char *, ...)
               __attribute__ ((__format__ (__printf__, 3, 4)));
int	_fclose_r(struct _reent *, FILE *);
int	_fcloseall_r(struct _reent *);
FILE *	_fdopen_r(struct _reent *, int, const char *);
int	_fflush_r(struct _reent *, FILE *);
char *  _fgets_r(struct _reent *, char *, int, FILE *);
int	_fiprintf_r(struct _reent *, FILE *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 3, 4)));
int	_fiscanf_r(struct _reent *, FILE *, const char *, ...)
               __attribute__ ((__format__ (__scanf__, 3, 4)));
FILE *	_fmemopen_r(struct _reent *, void *, size_t, const char *);
FILE *	_fopen_r(struct _reent *, const char *, const char *);
int	_fprintf_r(struct _reent *, FILE *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 3, 4)));
int	_fputc_r(struct _reent *, int, FILE *);
int	_fputs_r(struct _reent *, const char *, FILE *);
size_t	_fread_r(struct _reent *, void *, size_t _size, size_t _n, FILE *);
int	_fscanf_r(struct _reent *, FILE *, const char *, ...)
               __attribute__ ((__format__ (__scanf__, 3, 4)));
int	_fseek_r(struct _reent *, FILE *, long, int);
long	_ftell_r(struct _reent *, FILE *);
size_t	_fwrite_r(struct _reent *, const void * , size_t _size, size_t _n, FILE *);
int	_getc_r(struct _reent *, FILE *);
int	_getc_unlocked_r(struct _reent *, FILE *);
int	_getchar_r(struct _reent *);
int	_getchar_unlocked_r(struct _reent *);
char *	_gets_r(struct _reent *, char *);
int	_iprintf_r(struct _reent *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 2, 3)));
int	_iscanf_r(struct _reent *, const char *, ...)
               __attribute__ ((__format__ (__scanf__, 2, 3)));
int	_mkstemp_r(struct _reent *, char *);
char *	_mktemp_r(struct _reent *, char *);
FILE *	_open_memstream_r(struct _reent *, char **, size_t *);
void	_perror_r(struct _reent *, const char *);
int	_printf_r(struct _reent *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 2, 3)));
int	_putc_r(struct _reent *, int, FILE *);
int	_putc_unlocked_r(struct _reent *, int, FILE *);
int	_putchar_unlocked_r(struct _reent *, int);
int	_putchar_r(struct _reent *, int);
int	_puts_r(struct _reent *, const char *);
int	_remove_r(struct _reent *, const char *);
int	_rename_r(struct _reent *,
			   const char *_old, const char *_new);
int	_scanf_r(struct _reent *, const char *, ...)
               __attribute__ ((__format__ (__scanf__, 2, 3)));
int	_siprintf_r(struct _reent *, char *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 3, 4)));
int	_siscanf_r(struct _reent *, const char *, const char *, ...)
               __attribute__ ((__format__ (__scanf__, 3, 4)));
int	_sniprintf_r(struct _reent *, char *, size_t, const char *, ...)
               __attribute__ ((__format__ (__printf__, 4, 5)));
int	_snprintf_r(struct _reent *, char *, size_t, const char *, ...)
               __attribute__ ((__format__ (__printf__, 4, 5)));
int	_sprintf_r(struct _reent *, char *, const char *, ...)
               __attribute__ ((__format__ (__printf__, 3, 4)));
int	_sscanf_r(struct _reent *, const char *, const char *, ...)
               __attribute__ ((__format__ (__scanf__, 3, 4)));
char *	_tempnam_r(struct _reent *, const char *, const char *);
FILE *	_tmpfile_r(struct _reent *);
char *	_tmpnam_r(struct _reent *, char *);
int	_ungetc_r(struct _reent *, int, FILE *);
int	_vasiprintf_r(struct _reent *, char **, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 3, 0)));
char *	_vasniprintf_r(struct _reent*, char *, size_t *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 4, 0)));
char *	_vasnprintf_r(struct _reent*, char *, size_t *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 4, 0)));
int	_vasprintf_r(struct _reent *, char **, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 3, 0)));
int	_vdiprintf_r(struct _reent *, int, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 3, 0)));
int	_vdprintf_r(struct _reent *, int, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 3, 0)));
int	_vfiprintf_r(struct _reent *, FILE *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 3, 0)));
int	_vfiscanf_r(struct _reent *, FILE *, const char *, va_list)
               __attribute__ ((__format__ (__scanf__, 3, 0)));
int	_vfprintf_r(struct _reent *, FILE *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 3, 0)));
int	_vfscanf_r(struct _reent *, FILE *, const char *, va_list)
               __attribute__ ((__format__ (__scanf__, 3, 0)));
int	_viprintf_r(struct _reent *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 2, 0)));
int	_viscanf_r(struct _reent *, const char *, va_list)
               __attribute__ ((__format__ (__scanf__, 2, 0)));
int	_vprintf_r(struct _reent *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 2, 0)));
int	_vscanf_r(struct _reent *, const char *, va_list)
               __attribute__ ((__format__ (__scanf__, 2, 0)));
int	_vsiprintf_r(struct _reent *, char *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 3, 0)));
int	_vsiscanf_r(struct _reent *, const char *, const char *, va_list)
               __attribute__ ((__format__ (__scanf__, 3, 0)));
int	_vsniprintf_r(struct _reent *, char *, size_t, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 4, 0)));
int	_vsnprintf_r(struct _reent *, char *, size_t, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 4, 0)));
int	_vsprintf_r(struct _reent *, char *, const char *, va_list)
               __attribute__ ((__format__ (__printf__, 3, 0)));
int	_vsscanf_r(struct _reent *, const char *, const char *, va_list)
               __attribute__ ((__format__ (__scanf__, 3, 0)));

ssize_t __getdelim(char **, size_t *, int, FILE *);
ssize_t __getline(char **, size_t *, FILE *);

#ifdef __LARGE64_FILES
#if !defined(__CYGWIN__) || defined(_COMPILING_NEWLIB)
FILE *	fdopen64(int, const char *);
FILE *  fopen64(const char *, const char *);
_off64_t ftello64(FILE *);
_off64_t fseeko64(FILE *, _off64_t, int);
int     fgetpos64(FILE *, _fpos64_t *);
int     fsetpos64(FILE *, const _fpos64_t *);
FILE *  tmpfile64(void);

FILE *	_fdopen64_r(struct _reent *, int, const char *);
FILE *  _fopen64_r(struct _reent *,const char *, const char *);
_off64_t _ftello64_r(struct _reent *, FILE *);
_off64_t _fseeko64_r(struct _reent *, FILE *, _off64_t, int);
int     _fgetpos64_r(struct _reent *, FILE *, _fpos64_t *);
int     _fsetpos64_r(struct _reent *, FILE *, const _fpos64_t *);
FILE *  _tmpfile64_r(struct _reent *);
#endif /* !__CYGWIN__ */
#endif /* __LARGE64_FILES */

/*
 * Routines internal to the implementation.
 */

int	__srget_r(struct _reent *, FILE *);
int	__swbuf_r(struct _reent *, int, FILE *);

/*
 * Stdio function-access interface.
 */

#ifndef __STRICT_ANSI__
# ifdef __LARGE64_FILES
FILE	*funopen(const void * __cookie,
		int (*__readfn)(void * __c, char *__buf, int __n),
		int (*__writefn)(void * __c, const char *__buf, int __n),
		_fpos64_t (*__seekfn)(void * __c, _fpos64_t __off, int __whence),
		int (*__closefn)(void * __c));
# else
FILE	*funopen(const void * __cookie,
		 int (*__readfn)(void * __cookie, char *__buf, int __n),
		 int (*__writefn)(void * __cookie, const char *__buf, int __n),
		 fpos_t (*__seekfn)(void * __cookie, fpos_t __off, int __whence),
		 int (*__closefn)(void * __cookie));
# endif /* !__LARGE64_FILES */

# define	fropen(__cookie, __fn) funopen(__cookie, __fn, (int (*)())0, \
					       (fpos_t (*)())0, (int (*)())0)
# define	fwopen(__cookie, __fn) funopen(__cookie, (int (*)())0, __fn, \
					       (fpos_t (*)())0, (int (*)())0)

typedef ssize_t cookie_read_function_t(void *__cookie, char *__buf, size_t __n);
typedef ssize_t cookie_write_function_t(void *__cookie, const char *__buf,
					size_t __n);
# ifdef __LARGE64_FILES
typedef int cookie_seek_function_t(void *__cookie, _off64_t *__off,
				   int __whence);
# else
typedef int cookie_seek_function_t(void *__cookie, off_t *__off, int __whence);
# endif /* !__LARGE64_FILES */
typedef int cookie_close_function_t(void *__cookie);
typedef struct
{
  /* These four struct member names are dictated by Linux; hopefully,
     they don't conflict with any macros.  */
  cookie_read_function_t  *read;
  cookie_write_function_t *write;
  cookie_seek_function_t  *seek;
  cookie_close_function_t *close;
} cookie_io_functions_t;
FILE *fopencookie(void *__cookie, const char *__mode, cookie_io_functions_t __functions);
#endif /* ! __STRICT_ANSI__ */

#ifndef __CUSTOM_FILE_IO__
/*
 * The __sfoo macros are here so that we can 
 * define function versions in the C library.
 */
#define       __sgetc_raw_r(__ptr, __f) (--(__f)->_r < 0 ? __srget_r(__ptr, __f) : (int)(*(__f)->_p++))

#ifdef __SCLE
/*  For a platform with CR/LF, additional logic is required by
  __sgetc_r which would otherwise simply be a macro; therefore we
  use an inlined function.  The function is only meant to be inlined
  in place as used and the function body should never be emitted.  

  There are two possible means to this end when compiling with GCC,
  one when compiling with a standard C99 compiler, and for other
  compilers we're just stuck.  At the moment, this issue only
  affects the Cygwin target, so we'll most likely be using GCC.

  The traditional meaning of 'extern inline' for GCC is not
  to emit the function body unless the address is explicitly
  taken.  However this behaviour is changing to match the C99
  standard, which uses 'extern inline' to indicate that the
  function body *must* be emitted.  If we are using GCC, but do
  not have the new behaviour, we need to use extern inline; if
  we are using a new GCC with the C99-compatible behaviour, or
  a non-GCC compiler (which we will have to hope is C99, since
  there is no other way to achieve the effect of omitting the
  function if it isn't referenced) we just use plain 'inline',
  which c99 defines to mean more-or-less the same as the Gnu C
  'extern inline'.  */
#if defined(__GNUC__) && !defined(__GNUC_STDC_INLINE__)
/* We're using GCC, but without the new C99-compatible behaviour.  */
#define _ELIDABLE_INLINE extern __inline__ __attribute__ ((__always_inline__))
#else
/* We're using GCC in C99 mode, or an unknown compiler which 
  we just have to hope obeys the C99 semantics of inline.  */
#define _ELIDABLE_INLINE __inline__
#endif

_ELIDABLE_INLINE int __sgetc_r(struct _reent *__ptr, FILE *__p)
  {
    int __c = __sgetc_raw_r(__ptr, __p);
    if ((__p->_flags & __SCLE) && (__c == '\r'))
      {
      int __c2 = __sgetc_raw_r(__ptr, __p);
      if (__c2 == '\n')
        __c = __c2;
      else
        ungetc(__c2, __p);
      }
    return __c;
  }
#else
#define __sgetc_r(__ptr, __p) __sgetc_raw_r(__ptr, __p)
#endif

#ifdef _never /* __GNUC__ */
/* If this inline is actually used, then systems using coff debugging
   info get hopelessly confused.  21sept93 rich@cygnus.com.  */
_ELIDABLE_INLINE int __sputc_r(struct _reent *_ptr, int _c, FILE *_p) {
	if (--_p->_w >= 0 || (_p->_w >= _p->_lbfsize && (char)_c != '\n'))
		return (*_p->_p++ = _c);
	else
		return (__swbuf_r(_ptr, _c, _p));
}
#else
/*
 * This has been tuned to generate reasonable code on the vax using pcc
 */
#define       __sputc_raw_r(__ptr, __c, __p) \
	(--(__p)->_w < 0 ? \
		(__p)->_w >= (__p)->_lbfsize ? \
			(*(__p)->_p = (__c)), *(__p)->_p != '\n' ? \
				(int)*(__p)->_p++ : \
				__swbuf_r(__ptr, '\n', __p) : \
			__swbuf_r(__ptr, (int)(__c), __p) : \
		(*(__p)->_p = (__c), (int)*(__p)->_p++))
#ifdef __SCLE
#define __sputc_r(__ptr, __c, __p) \
        ((((__p)->_flags & __SCLE) && ((__c) == '\n')) \
          ? __sputc_raw_r(__ptr, '\r', (__p)) : 0 , \
        __sputc_raw_r((__ptr), (__c), (__p)))
#else
#define __sputc_r(__ptr, __c, __p) __sputc_raw_r(__ptr, __c, __p)
#endif
#endif

#define	__sfeof(p)	(((p)->_flags & __SEOF) != 0)
#define	__sferror(p)	(((p)->_flags & __SERR) != 0)
#define	__sclearerr(p)	((void)((p)->_flags &= ~(__SERR|__SEOF)))
#define	__sfileno(p)	((p)->_file)

#ifndef _REENT_SMALL
#define	feof(p)		__sfeof(p)
#define	ferror(p)	__sferror(p)
#define	clearerr(p)	__sclearerr(p)
#endif

#if 0 /*ndef __STRICT_ANSI__ - FIXME: must initialize stdio first, use fn */
#define	fileno(p)	__sfileno(p)
#endif

#ifndef __CYGWIN__
#ifndef lint
#define	getc(fp)	__sgetc_r(_REENT, fp)
#define putc(x, fp)	__sputc_r(_REENT, x, fp)
#endif /* lint */
#endif /* __CYGWIN__ */

#ifndef __STRICT_ANSI__
/* fast always-buffered version, true iff error */
#define	fast_putc(x,p) (--(p)->_w < 0 ? \
	__swbuf_r(_REENT, (int)(x), p) == EOF : (*(p)->_p = (x), (p)->_p++, 0))

#define	L_cuserid	9		/* posix says it goes in stdio.h :( */
#ifdef __CYGWIN__
#define L_ctermid       16
#endif
#endif

#endif /* !__CUSTOM_FILE_IO__ */

#define	getchar()	getc(stdin)
#define	putchar(x)	putc(x, stdout)

_END_STD_C

#endif /* _STDIO_H_ */
