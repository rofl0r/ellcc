add_library(reent # SHARED
    closer.c  fstat64r.c  gettimeofdayr.c  linkr.c     open64r.c  reent.c    signalr.c  timesr.c
    execr.c   fstatr.c    impure.c         lseek64r.c  openr.c    renamer.c  signgam.c  unlinkr.c
    fcntlr.c  getreent.c  isattyr.c        lseekr.c    readr.c    sbrkr.c    statr.c    writer.c)

add_custom_command(TARGET reent POST_BUILD
    COMMAND llvm-ar r ${LIBECC_DIR}/../../lib/libecc.a $(reent_OBJECTS))
#install_targets(/lib basic)
