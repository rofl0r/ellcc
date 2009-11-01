
// $Id: builtin-t.h 2397 2006-07-11 03:51:55Z quarl $

// header file declaring minimal types needed for GCC builtins.

typedef struct _IO_FILE FILE;
typedef unsigned long size_t;
typedef int ssize_t;
typedef __builtin_va_list va_list;
typedef void *__malloc_ptr_t;
struct tm;
#define complex         _Complex
