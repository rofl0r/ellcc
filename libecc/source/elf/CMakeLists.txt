add_library(target # SHARED
    brk.c close.c errno.c exit.c fstat.c fcntl.c getpid.c impure.c kill.c
    lseek.c lstat.c open.c read.c stat.c unlink.c write.c)

add_custom_command(TARGET target POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
    COMMAND cp libtarget.a ${LIBECC_DIR}/../lib/${ELLCC_TARGET})

add_subdirectory(crt)
#install_targets(/lib basic)
