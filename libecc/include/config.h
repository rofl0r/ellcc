#ifndef _CONFIG_H_

/* libecc configuration parameters.
 */

/* Define if reentrant system calls are provided.
 */
#define REENTRANT_SYSCALLS_PROVIDED

/** Define for a single threaded environment.
 */
// #define __SINGLE_THREAD__
/** Define if the stat structure has blksize.
 */
#define HAVE_BLKSIZE
/** Enable multi-byte characters.
 */
// #define _MB_CAPABLE
/** Define for formatting with positional arguments.
 */
// #define _WANT_IO_POS_ARGS
/** Define for formatting with long longs.
 */
#define _WANT_IO_LONG_LONG
/** Define for formatting with long doubles.
 */
// #define _WANT_IO_LONG_DOUBLE
/** Define for formatting with C99 semantics.
 */
#define _WANT_IO_C99_FORMATS
/** Build a compact version of ctype.
 */
// #define COMPACT_CTYPE
/** Define if the nanosleep function exists.
 */
#define HAVE_NANOSLEEP
/** Define if _system exists elsewhere.
 */
// #define HAVE_SYSTEM
/** Define if exec does not exist.
 */
// #define NO_EXEC
/** Define if getopt exists elsewhere.
 */
// #define HAVE_GETOPT
/** Define atexit idata should be freed on exit.
 */
// #define _ATEXIT_DYNAMIC_ALLOC
/** Define if malloc does not exist or exists elsewhere.
 */
// #define MALLOC_PROVIDED

/** Define if opendir does not exist or exists elsewhere.
 */
#define HAVE_OPENDIR
/** Define if locale should be used.
 */
#define USE_LOCALE
/** Define if dirent contains d_namelen.
 */
// #define _DIRENT_HAVE_D_NAMLEN

/** Define if reentrant system calls are provided outside of libecc.
 */
// #define REENTRANT_SYSCALLS_PROVIDED
/** Build a small version of reentrant functions.
 */
// #define _REENT_SMALL
/** Define if reentrant system calls are provided outside of libecc.
 */
// #define REENTRANT_SYSCALLS_PROVIDED
/** Define if a DIR has a loc.
 */
// #define HAVE_DD_LOCK

#endif
