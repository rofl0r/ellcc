#include <syscall.h>
#include <signal.h>
#include <string.h>


#include <stdio.h>
#include <asm/unistd.h>

extern void sigreturnentry (void) asm ("__sigreturnentry");
static void sigreturnstub(void) __attribute__((__used__));
static void sigreturnstub(void)
{
    asm volatile ("__sigreturnentry:");
    (void)INLINE_SYSCALL(rt_sigreturn,1, 0);
}

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
    // The kernel has a different view of struct sigaction.
#if defined(__mips__)
    struct {
        unsigned long sa_flags;
        void (*_sa_handler)(int);
        sigset_t sa_mask;
    } kact, koldact;
#define NO_RESTORER 1
#else
    struct {
        void (*_sa_handler)(int);
        unsigned long sa_flags;
        void (*sa_restorer)(void);
        sigset_t sa_mask;
    } kact, koldact;
#endif

    if (act) {
        kact._sa_handler = act->sa_handler;
#if NO_RESTORER
        kact.sa_flags = act->sa_flags;
#else
        kact.sa_flags = act->sa_flags | 0x04000000;
        kact.sa_restorer = sigreturnentry;
#endif
        memcpy(&kact.sa_mask, &act->sa_mask, sizeof(sigset_t));
    }
    int result = INLINE_SYSCALL(rt_sigaction, 4, signum,
                                act ? &kact : NULL, oldact ? &koldact : NULL,
                                _NSIG / 8);
    if (result >= 0 && oldact) {
        oldact->sa_handler = koldact._sa_handler;
        oldact->sa_flags = koldact.sa_flags;
#if !NO_RESTORER
        oldact->sa_restorer = koldact.sa_restorer;
#endif
        memcpy(&oldact->sa_mask, &koldact.sa_mask, sizeof(sigset_t));
    }
    return result;
}

