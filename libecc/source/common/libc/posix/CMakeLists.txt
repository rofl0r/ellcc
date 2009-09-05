add_library(posix
    closedir.c  execle.c  execve.c   isatty.c   readdir.c    scandir.c  telldir.c
    creat.c     execlp.c  execvp.c   opendir.c  readdir_r.c  seekdir.c  usleep.c
    execl.c     execv.c   _isatty.c  popen.c    rewinddir.c  sleep.c)

add_custom_command(TARGET posix POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
            COMMAND ${TARGET_AR} ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a
                    $(posix_OBJECTS))
