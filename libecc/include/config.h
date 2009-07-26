#ifndef _CONFIG_H_

/** libecc configuration parameters.
 */

/** Define for a single threaded environment.
 */
// #define __SINGLE_THREAD__
/** Define if the stat structure has blksize.
 */
#define HAVE_BLKSIZE
/** Enable multi-byte characters.
 */
#define _MB_CAPABLE
/** Define for integer only formatting.
 */
// #define INTEGER_ONLY
/** Define for no floating point formatting.
 */
// #define NO_FLOATING_POINT
/** Define for string (no file) only formatting.
 */
// #define STRING_ONLY
/** Define for formatting with positional arguments.
 */
#define _WANT_IO_POS_ARGS
/** Define for formatting with long longs.
 */
#define _WANT_IO_LONG_LONG
/** Define for formatting with long doubles.
 */
#define _WANT_IO_LONG_DOUBLE
/** Define for formatting with C99 semantics.
 */
#define _WANT_IO_LONG_DOUBLE
/** Build a compact version of ctype.
 */
// #define COMPACT_CTYPE
/** Define if the nanosleep function exists.
 */
#define HAVE_NANOSLEEP

/** Define if malloc does not exist or exists elsewhere.
 */
// #define MALLOC_PROVIDED

/** Define if opendir does not exist or exists elsewhere.
 */
#define HAVE_OPENDIR
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
