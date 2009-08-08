add_library(reent
    closer.c  gettimeofdayr.c  linkr.c   reent.c    signalr.c  timesr.c
    execr.c   fstatr.c         impure.c  openr.c    renamer.c  signgam.c  unlinkr.c
    fcntlr.c  getreent.c       isattyr.c lseekr.c   readr.c    sbrkr.c    statr.c    writer.c)

ELLCC_SETUP_TARGET(reent)
