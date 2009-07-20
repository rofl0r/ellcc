add_library(syscalls # SHARED
    sysclose.c   sysfork.c    sysgettod.c  syslink.c   sysread.c  systimes.c   syswrite.c
    sysexecve.c  sysfstat.c   sysisatty.c  syslseek.c  syssbrk.c  sysunlink.c
    sysfcntl.c   sysgetpid.c  syskill.c    sysopen.c   sysstat.c  syswait.c)

add_custom_command(TARGET syscalls POST_BUILD
    COMMAND llvm-ar r ${LIBECC_DIR}/../../lib/libecc.a $(syscalls_OBJECTS))
#install_targets(/lib basic)
