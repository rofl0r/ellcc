add_library(reent # SHARED
    closer.c  fstat64r.c  gettimeofdayr.c  linkr.c     open64r.c  reent.c    signalr.c  timesr.c
    execr.c   fstatr.c    impure.c         lseek64r.c  openr.c    renamer.c  signgam.c  unlinkr.c
    fcntlr.c  getreent.c  isattyr.c        lseekr.c    readr.c    sbrkr.c    statr.c    writer.c)

#install_targets(/lib basic)
