/*
 *	Common definitions for atexit-like routines
 */

enum __atexit_types
{
  __et_atexit,
  __et_onexit,
  __et_cxa
};

#define _ATEXIT_SIZE 32 /* must be at least 32 to guarantee ANSI conformance */

struct _on_exit_args {
        void *  _fnargs[_ATEXIT_SIZE];          /* user fn args */
        void *  _dso_handle[_ATEXIT_SIZE];
        /* Bitmask is set if user function takes arguments.  */
        unsigned long _fntypes;                       /* type of exit routine -
                                   Must have at least _ATEXIT_SIZE bits */
        /* Bitmask is set if function was registered via __cxa_atexit.  */
        unsigned long _is_cxa;
};

#ifdef _REENT_SMALL
struct _atexit {
        struct  _atexit *_next;                 /* next in list */
        int     _ind;                           /* next index in this table */
        void    (*_fns[_ATEXIT_SIZE])(void);    /* the table itself */
        struct _on_exit_args * _on_exit_args_ptr;
};
#else
struct _atexit {
        struct  _atexit *_next;                 /* next in list */
        int     _ind;                           /* next index in this table */
        /* Some entries may already have been called, and will be NULL.  */
        void    (*_fns[_ATEXIT_SIZE])(void);    /* the table itself */
        struct _on_exit_args _on_exit_args;
};
#endif
 
void __call_exitprocs(int, void *);
int __register_exitproc(int, void (*fn) (void), void *, void *);
