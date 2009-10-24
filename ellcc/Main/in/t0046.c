#if 0


 















































































 



 








 



 








 








 





 
 




 
 

 


 
 

 
 

 
 

 
 

 
 

 
 

 


 
 

 
 

 
 

 


 




 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 



 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 




























































 


















































































































 
























































































































































 






 
















































 





 

















 






















































 



















 































 










 










 
 





















































 







 




















 





 







 






 










 

 








typedef signed char __int8_t ;
typedef unsigned char __uint8_t ;








typedef signed short __int16_t;
typedef unsigned short __uint16_t;








typedef __int16_t __int_least16_t;
typedef __uint16_t __uint_least16_t;










typedef signed int __int32_t;
typedef unsigned int __uint32_t;
















typedef __int32_t __int_least32_t;
typedef __uint32_t __uint_least32_t;





















typedef signed long long __int64_t;
typedef unsigned long long __uint64_t;


 




























 

typedef int _LOCK_T;
typedef int _LOCK_RECURSIVE_T;



















typedef long _off_t;







typedef short __dev_t;




typedef unsigned short __uid_t;


typedef unsigned short __gid_t;



__extension__ typedef long long _off64_t;





 

typedef long _fpos_t;  
  




typedef _off64_t _fpos64_t;





typedef int _ssize_t;

























 






 



 




 










 

 



 






 




























 









 

















 

 

 



































 

 








































































 

 






























































































typedef unsigned int wint_t;





 
 































 




























 
typedef struct
{
 int __count;
 union
 {
 wint_t __wch;
 unsigned char __wchb[4];
 } __value;  
} _mbstate_t;



typedef _LOCK_RECURSIVE_T _flock_t;



 
typedef void *_iconv_t;











typedef unsigned long __ULong;















struct _reent;




 

struct _Bigint 
{
 struct _Bigint *_next;
 int _k, _maxwds, _sign, _wds;
 __ULong _x[1];
};

 
struct __tm
{
 int __tm_sec;
 int __tm_min;
 int __tm_hour;
 int __tm_mday;
 int __tm_mon;
 int __tm_year;
 int __tm_wday;
 int __tm_yday;
 int __tm_isdst;
};



 



struct _on_exit_args {
 void * _fnargs[32];  
 void * _dso_handle[32];
  
 __ULong _fntypes;  
  
 __ULong _is_cxa;
};









struct _atexit {
 struct _atexit *_next;  
 int _ind;  
  
 void (*_fns[32])(void);  
 struct _on_exit_args _on_exit_args;
};







 

struct __sbuf {
 unsigned char *_base;
 int _size;
};























 
























struct __sFILE {
 unsigned char *_p;  
 int _r;  
 int _w;  
 short _flags;  
 short _file;  
 struct __sbuf _bf;  
 int _lbfsize;  





  
 void * _cookie;  

 _ssize_t (*
_read) (struct _reent *, void *, char *, int);
 _ssize_t (*
_write) (struct _reent *, void *, const char *, int);
 _fpos_t (*_seek) (struct _reent *, void *, _fpos_t, int);
 int (*_close) (struct _reent *, void *);

  
 struct __sbuf _ub;  
 unsigned char *_up;  
 int _ur;  

  
 unsigned char _ubuf[3];  
 unsigned char _nbuf[1];  

  
 struct __sbuf _lb;  

  
 int _blksize;  
 int _offset;  


 struct _reent *_data;  



 _flock_t _lock;  

};








struct __sFILE64 {
 unsigned char *_p;  
 int _r;  
 int _w;  
 short _flags;  
 short _file;  
 struct __sbuf _bf;  
 int _lbfsize;  

 struct _reent *_data;

  
 void * _cookie;  

 _ssize_t (*
_read) (struct _reent *, void *, char *, int);
 _ssize_t (*
_write) (struct _reent *, void *, const char *, int);
 _fpos_t (*_seek) (struct _reent *, void *, _fpos_t, int);
 int (*_close) (struct _reent *, void *);

  
 struct __sbuf _ub;  
 unsigned char *_up;  
 int _ur;  

  
 unsigned char _ubuf[3];  
 unsigned char _nbuf[1];  

  
 struct __sbuf _lb;  

  
 int _blksize;  
 int _flags2;  

 _off64_t _offset;  
 _fpos64_t (*_seek64) (struct _reent *, void *, _fpos64_t, int);


 _flock_t _lock;  

};
typedef struct __sFILE64 __FILE;





struct _glue 
{
 struct _glue *_next;
 int _niobs;
 __FILE *_iobs;
};














 







struct _rand48 {
 unsigned short _seed[3];
 unsigned short _mult[3];
 unsigned short _add;




};

 










 














































































































































struct _reent
{
 int _errno;  

 

 
 __FILE *_stdin, *_stdout, *_stderr;

 int _inc;  
 char _emergency[25];
 
 int _current_category;  
 const char *_current_locale;

 int __sdidinit;  

 void (*__cleanup) (struct _reent *);

  
 struct _Bigint *_result;
 int _result_k;
 struct _Bigint *_p5s;
 struct _Bigint **_freelist;

  
 int _cvtlen;  
 char *_cvtbuf;

 union
 {
 struct
 {
 unsigned int _unused_rand;
 char * _strtok_last;
 char _asctime_buf[26];
 struct __tm _localtime_buf;
 int _gamma_signgam;
 __extension__ unsigned long long _rand_next;
 struct _rand48 _r48;
 _mbstate_t _mblen_state;
 _mbstate_t _mbtowc_state;
 _mbstate_t _wctomb_state;
 char _l64a_buf[8];
 char _signal_buf[24];
 int _getdate_err; 
 _mbstate_t _mbrlen_state;
 _mbstate_t _mbrtowc_state;
 _mbstate_t _mbsrtowcs_state;
 _mbstate_t _wcrtomb_state;
 _mbstate_t _wcsrtombs_state;
 int _h_errno;
 } _reent;
 

 
 struct
 {

 unsigned char * _nextf[30];
 unsigned int _nmalloc[30];
 } _unused;
 } _new;

  
 struct _atexit *_atexit;  
 struct _atexit _atexit0;  

  
 void (**(_sig_func))(int);

 

 
 struct _glue __sglue;  
 __FILE __sf[3];  
};






















































































































































 





extern struct _reent *_impure_ptr ;
extern struct _reent *const _global_impure_ptr ;

void _reclaim_reent (struct _reent *);

 





 struct _reent * __getreent (void);

















 





 























































































 














































 






 



 




 










 

 



 






 




























 









 

















 

 

 




















typedef int ptrdiff_t;









 





 

 











































typedef unsigned int size_t;





























 

 



































































































 
 































 



























 
struct stat;
struct tms;
struct timeval;
struct timezone;






























 

extern int _close_r (struct _reent *, int);
extern int _execve_r (struct _reent *, char *, char **, char **);
extern int _fcntl_r (struct _reent *, int, int, int);
extern int _fork_r (struct _reent *);
extern int _fstat_r (struct _reent *, int, struct stat *);
extern int _getpid_r (struct _reent *);
extern int _isatty_r (struct _reent *, int);
extern int _kill_r (struct _reent *, int, int);
extern int _link_r (struct _reent *, const char *, const char *);
extern _off_t _lseek_r (struct _reent *, int, _off_t, int);
extern int _open_r (struct _reent *, const char *, int, int);
extern _ssize_t _read_r (struct _reent *, int, void *, size_t);
extern void *_sbrk_r (struct _reent *, ptrdiff_t);
extern int _stat_r (struct _reent *, const char *, struct stat *);
extern unsigned long _times_r (struct _reent *, struct tms *);
extern int _unlink_r (struct _reent *, const char *);
extern int _wait_r (struct _reent *, int *);
extern _ssize_t _write_r (struct _reent *, int, const void *, size_t);

 
extern int _gettimeofday_r (struct _reent *, struct timeval *__tp, void *__tzp);







struct stat64;

extern _off64_t _lseek64_r (struct _reent *, int, _off64_t, int);
extern int _fstat64_r (struct _reent *, int, struct stat64 *);
extern int _open64_r (struct _reent *, const char *, int, int);











 







#endif
typedef unsigned long __ULong;

struct _Bigint 
{
 struct _Bigint *_next;
 int _k, _maxwds, _sign, _wds;
 __ULong _x[1];
};

struct _rand48 {
 unsigned short _seed[3];
 unsigned short _mult[3];
 unsigned short _add;




};

typedef unsigned int wint_t;
typedef struct
{
 int __count;
 union
 {
 wint_t __wch;
 unsigned char __wchb[4];
 } __value;  
} _mbstate_t;


struct __tm
{
 int __tm_sec;
 int __tm_min;
 int __tm_hour;
 int __tm_mday;
 int __tm_mon;
 int __tm_year;
 int __tm_wday;
 int __tm_yday;
 int __tm_isdst;
};

struct _on_exit_args {
 void * _fnargs[32];  
 void * _dso_handle[32];
  
 __ULong _fntypes;  
  
 __ULong _is_cxa;
};

struct _atexit {
 struct _atexit *_next;  
 int _ind;  
  
 void (*_fns[32])(void);  
 struct _on_exit_args _on_exit_args;
};

struct __sbuf {
 unsigned char *_base;
 int _size;
};

typedef long _fpos_t;  
typedef int _ssize_t;
__extension__ typedef long long _off64_t;
typedef _off64_t _fpos64_t;

typedef int _LOCK_RECURSIVE_T;
typedef _LOCK_RECURSIVE_T _flock_t;

struct __sFILE64 {
 unsigned char *_p;  
 int _r;  
 int _w;  
 short _flags;  
 short _file;  
 struct __sbuf _bf;  
 int _lbfsize;  

 struct _reent *_data;

  
 void * _cookie;  

 _ssize_t (*
_read) (struct _reent *, void *, char *, int);
 _ssize_t (*
_write) (struct _reent *, void *, const char *, int);
 _fpos_t (*_seek) (struct _reent *, void *, _fpos_t, int);
 int (*_close) (struct _reent *, void *);

  
 struct __sbuf _ub;  
 unsigned char *_up;  
 int _ur;  

  
 unsigned char _ubuf[3];  
 unsigned char _nbuf[1];  

  
 struct __sbuf _lb;  

  
 int _blksize;  
 int _flags2;  

 _off64_t _offset;  
 _fpos64_t (*_seek64) (struct _reent *, void *, _fpos64_t, int);


 _flock_t _lock;  

};
typedef struct __sFILE64 __FILE;

struct _glue 
{
 struct _glue *_next;
 int _niobs;
 __FILE *_iobs;
};

struct _reent
{
 int _errno;  

 

 
 __FILE *_stdin, *_stdout, *_stderr;

 int _inc;  
 char _emergency[25];
 
 int _current_category;  
 const char *_current_locale;

 int __sdidinit;  

 void (*__cleanup) (struct _reent *);

  
 struct _Bigint *_result;
 int _result_k;
 struct _Bigint *_p5s;
 struct _Bigint **_freelist;

  
 int _cvtlen;  
 char *_cvtbuf;

 union
 {
 struct
 {
 unsigned int _unused_rand;
 char * _strtok_last;
 char _asctime_buf[26];
 struct __tm _localtime_buf;
 int _gamma_signgam;
 __extension__ unsigned long long _rand_next;
 struct _rand48 _r48;
 _mbstate_t _mblen_state;
 _mbstate_t _mbtowc_state;
 _mbstate_t _wctomb_state;
 char _l64a_buf[8];
 char _signal_buf[24];
 int _getdate_err; 
 _mbstate_t _mbrlen_state;
 _mbstate_t _mbrtowc_state;
 _mbstate_t _mbsrtowcs_state;
 _mbstate_t _wcrtomb_state;
 _mbstate_t _wcsrtombs_state;
 int _h_errno;
 } _reent;
 

 
 struct
 {

 unsigned char * _nextf[30];
 unsigned int _nmalloc[30];
 } _unused;
 } _new;

  
 struct _atexit *_atexit;  
 struct _atexit _atexit0;  

  
 void (**(_sig_func))(int);

 

 
 struct _glue __sglue;  
 __FILE __sf[3];  
};

static struct _reent  impure_data = {
    0,
    &(impure_data).__sf[0],
    &(impure_data).__sf[1],
    &(impure_data).__sf[2],
    0,
    "",
    0,
    "C",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    {
        {
            0,
            0,
            "",
            {
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0
            },
            0,
            1,
            {
                {
                    (0x330e),
                    (0xabcd),
                    (0x1234)
                },
                {
                    (0xe66d),
                    (0xdeec),
                    (0x0005)
                },
                (0x000b)
            },
            {
                0,
                {
                    0
                }
            },
            {
                0,
                {
                    0
                }
            },
            {
                0,
                {
                    0
                }
            },
            "",
            "",
            0,
            {
                0,
                {
                    0
                }
            },
            {
                0,
                {
                    0
                }
            },
            {
                0,
                {
                    0
                }
            },
            {
                0,
                {
                    0
                }
            },
            {
                0,
                {
                    0
                }
            }
        }
    },
    0,
    {
        0,
        0,
        {
            0
        },
        {
            {
                0
            },
            {
                0
            },
            0,
            0
        }
    },
    0,
    {
        0,
        0,
        0
    }
};

int main()
{
}
