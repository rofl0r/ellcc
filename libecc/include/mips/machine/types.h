#ifndef	_MACHINE_TYPES_H_
#define	_MACHINE_TYPES_H_

#define	_CLOCK_T_	unsigned long		/* clock() */
#define	_TIME_T_	long			/* time() */
#define _CLOCKID_T_ 	unsigned long
#define _TIMER_T_   	unsigned long

typedef long int __off_t;
typedef int __pid_t;
__extension__ typedef long long int __loff_t;
typedef unsigned long long __dev_t;     // RICH: Check this.
typedef unsigned int __uid_t;     // RICH: Check this.
typedef unsigned int __gid_t;     // RICH: Check this.
typedef long __ssize_t;     // RICH: Check this.


#endif // _MACHINE_TYPES_H_


