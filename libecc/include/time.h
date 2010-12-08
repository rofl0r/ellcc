/*
 * time.h
 * 
 * Struct and function declarations for dealing with time.
 */

#ifndef _TIME_H_
#define _TIME_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define	NULL	0
#endif

/* Get _CLOCKS_PER_SEC_ */
// RICH #include <machine/time.h>

#ifndef _CLOCKS_PER_SEC_
#define _CLOCKS_PER_SEC_ 1000
#endif

#define CLOCKS_PER_SEC _CLOCKS_PER_SEC_
#define CLK_TCK CLOCKS_PER_SEC
#define __need_size_t
#include <stddef.h>

#include <sys/types.h>

struct tm
{
  int	tm_sec;
  int	tm_min;
  int	tm_hour;
  int	tm_mday;
  int	tm_mon;
  int	tm_year;
  int	tm_wday;
  int	tm_yday;
  int	tm_isdst;
};

clock_t	   clock(void);
double	   difftime(time_t _time2, time_t _time1);
time_t	   mktime(struct tm *_timeptr);
time_t	   time(time_t *_timer);
char	  *asctime(const struct tm *_tblock);
char	  *ctime(const time_t *_time);
struct tm *gmtime(const time_t *_timer);
struct tm *localtime(const time_t *_timer);
size_t	   strftime(char *_s, size_t _maxsize, const char *_fmt, const struct tm *_t);

char	  *asctime_r(const struct tm *, char *);
char	  *ctime_r(const time_t *, char *);
struct tm *gmtime_r(const time_t *, struct tm *);
struct tm *localtime_r(const time_t *, struct tm *);

#ifndef __STRICT_ANSI__
char      *strptime(const char *, const char *, struct tm *);
void      tzset(void);
void      _tzset_r(struct _reent *);

typedef struct __tzrule_struct
{
  char ch;
  int m;
  int n;
  int d;
  int s;
  time_t change;
  long offset; /* Match type of _timezone. */
} __tzrule_type;

typedef struct __tzinfo_struct
{
  int __tznorth;
  int __tzyear;
  __tzrule_type __tzrule[2];
} __tzinfo_type;

__tzinfo_type *__gettzinfo(void);

/* getdate functions */

#define getdate_err (*__getdate_err())
int *__getdate_err(void);

struct tm *	getdate(const char *);
/* getdate_err is set to one of the following values to indicate the error.
     1  the DATEMSK environment variable is null or undefined,
     2  the template file cannot be opened for reading,
     3  failed to get file status information,
     4  the template file is not a regular file,
     5  an error is encountered while reading the template file,
     6  memory allication failed (not enough memory available),
     7  there is no line in the template that matches the input,
     8  invalid input specification  */

/* getdate_r returns the error code as above */
int		getdate_r(const char *, struct tm *);

/* defines for the opengroup specifications Derived from Issue 1 of the SVID.  */
extern long _timezone;
extern int _daylight;
extern char *_tzname[2];

/* POSIX defines the external tzname being defined in time.h */
#ifndef tzname
#define tzname _tzname
#endif
#endif /* !__STRICT_ANSI__ */

#include <signal.h>

/* Clocks, P1003.1b-1993, p. 263 */

int clock_settime(clockid_t clock_id, const struct timespec *tp);
int clock_gettime(clockid_t clock_id, struct timespec *tp);
int clock_getres(clockid_t clock_id, struct timespec *res);

/* Create a Per-Process Timer, P1003.1b-1993, p. 264 */

int timer_create(clockid_t clock_id, struct sigevent *evp, timer_t *timerid);

/* Delete a Per_process Timer, P1003.1b-1993, p. 266 */

int timer_delete(timer_t timerid);

/* Per-Process Timers, P1003.1b-1993, p. 267 */

int timer_settime(timer_t timerid, int flags, const struct itimerspec *value,
   struct itimerspec *ovalue);
int timer_gettime(timer_t timerid, struct itimerspec *value);
int timer_getoverrun(timer_t timerid);

/* High Resolution Sleep, P1003.1b-1993, p. 269 */

int nanosleep(const struct timespec  *rqtp, struct timespec *rmtp);

/* CPU-time Clock Attributes, P1003.4b/D8, p. 54 */

/* values for the clock enable attribute */

#define CLOCK_ENABLED  1  /* clock is enabled, i.e. counting execution time */
#define CLOCK_DISABLED 0  /* clock is disabled */

/* values for the pthread cputime_clock_allowed attribute */

#define CLOCK_ALLOWED    1 /* If a thread is created with this value a */
                           /*   CPU-time clock attached to that thread */
                           /*   shall be accessible. */
#define CLOCK_DISALLOWED 0 /* If a thread is created with this value, the */
                           /*   thread shall not have a CPU-time clock */
                           /*   accessible. */

/* Manifest Constants, P1003.1b-1993, p. 262 */

#define CLOCK_REALTIME (clockid_t)1

/* Flag indicating time is "absolute" with respect to the clock
   associated with a time.  */

#define TIMER_ABSTIME	4

/* Manifest Constants, P1003.4b/D8, p. 55 */

/* When used in a clock or timer function call, this is interpreted as
   the identifier of the CPU_time clock associated with the PROCESS
   making the function call.  */

#define CLOCK_PROCESS_CPUTIME (clockid_t)2

/*  When used in a clock or timer function call, this is interpreted as
    the identifier of the CPU_time clock associated with the THREAD
    making the function call.  */

#define CLOCK_THREAD_CPUTIME (clockid_t)3

/* Accessing a Process CPU-time CLock, P1003.4b/D8, p. 55 */

int clock_getcpuclockid(pid_t pid, clockid_t *clock_id);


/* CPU-time Clock Attribute Access, P1003.4b/D8, p. 56 */

int clock_setenable_attr(clockid_t clock_id, int attr);
int clock_getenable_attr(clockid_t clock_id, int *attr);

#ifdef __cplusplus
}
#endif

#endif /* _TIME_H_ */

