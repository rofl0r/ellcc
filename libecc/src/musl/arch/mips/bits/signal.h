#if defined(_POSIX_SOURCE) || defined(_POSIX_C_SOURCE) \
 || defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE)

typedef struct {
	unsigned long __regs[21];
} mcontext_t;

typedef struct __ucontext {
	unsigned long uc_flags;
	struct __ucontext *uc_link;
	stack_t uc_stack;
	mcontext_t uc_mcontext;
	sigset_t uc_sigmask;
	unsigned long uc_regspace[128];
} ucontext_t;

#define SA_NOCLDSTOP  1
#define SA_NOCLDWAIT  2
#define SA_SIGINFO    4
#define SA_ONSTACK    0x08000000
#define SA_RESTART    0x10000000
#define SA_NODEFER    0x40000000
#define SA_RESETHAND  0x80000000
#define SA_RESTORER   0x04000000

#ifdef _GNU_SOURCE
struct sigcontext
{
	unsigned long trap_no, error_code, oldmask;
	unsigned long arm_r0, arm_r1, arm_r2, arm_r3;
	unsigned long arm_r4, arm_r5, arm_r6, arm_r7;
	unsigned long arm_r8, arm_r9, arm_r10, arm_fp;
	unsigned long arm_ip, arm_sp, arm_lr, arm_pc;
	unsigned long arm_cpsr, fault_address;
};
#define NSIG      64
#endif

#endif

#define SIGHUP    1
#define SIGINT    2
#define SIGQUIT   3
#define SIGILL    4
#define SIGTRAP   5
#define SIGABRT   6
#define SIGIOT    SIGABRT
#define SIGSTKFLT 7
#define SIGFPE    8
#define SIGKILL   9
#define SIGBUS    10
#define SIGSEGV   11
#define SIGSYS    12
#define SIGPIPE   13
#define SIGALRM   14
#define SIGTERM   15
#define SIGUSR1   16
#define SIGUSR2   17
#define SIGCHLD   18
#define SIGPWR    19
#define SIGWINCH  20
#define SIGURG    21
#define SIGIO     22
#define SIGPOLL   SIGIO
#define SIGSTOP   23
#define SIGTSTP   24
#define SIGCONT   25
#define SIGTTIN   26
#define SIGTTOU   27
#define SIGVTALRM 28
#define SIGPROF   29
#define SIGXCPU   30
#define SIGXFSZ   31
#define SIGUNUSED SIGSYS

#ifdef _BSD_SOURCE
#define SIGINFO         SIGUSR1 /* For NetBSD compatability */
#endif
