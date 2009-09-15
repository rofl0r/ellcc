add_library(reent
    closer.c  gettimeofdayr.c  linkr.c   reent.c    signalr.c  timesr.c
    execr.c   fstatr.c         openr.c   renamer.c  signgam.c  unlinkr.c
    fcntlr.c  getreent.c       isattyr.c lseekr.c   readr.c    sbrkr.c    statr.c    writer.c)

add_custom_command(TARGET reent POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
            COMMAND ${TARGET_AR} ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a
                    $(reent_OBJECTS))
