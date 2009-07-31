add_library(target # SHARED
    _brk.c _close.c _exit.c _fstat.c _lseek.c _read.c _write.c)

add_custom_command(TARGET target POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
    COMMAND cp libtarget.a ${LIBECC_DIR}/../lib/${ELLCC_TARGET})

add_subdirectory(crt)
#install_targets(/lib basic)
