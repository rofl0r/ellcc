add_library(target # SHARED
    _brk.c _close.c __errno.c _exit.c _fstat.c _fcntl.c _getpid.c _kill.c
    _lseek.c _lstat.c _open.c _read.c _stat.c _write.c)

add_custom_command(TARGET target POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
    COMMAND cp libtarget.a ${LIBECC_DIR}/../lib/${ELLCC_TARGET})

add_subdirectory(crt)
#install_targets(/lib basic)
