add_library(syscalls
    sysclose.c   sysfork.c    sysgettod.c  syslink.c   sysread.c  systimes.c   syswrite.c
    sysexecve.c  sysfstat.c   sysisatty.c  syslseek.c  syssbrk.c  sysunlink.c
    sysfcntl.c   sysgetpid.c  syskill.c    sysopen.c   sysstat.c  syswait.c)

ELLCC_SETUP_TARGET(syscalls)
