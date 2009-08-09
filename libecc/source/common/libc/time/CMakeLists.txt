add_library(time
    asctime.c    ctime.c     gettzinfo.c  lcltime.c    mktime.c    strptime.c  tzlock.c   tzvars.c
    asctime_r.c  ctime_r.c   gmtime.c     lcltime_r.c  mktm_r.c    time.c      tzset.c
    clock.c      difftime.c  gmtime_r.c   local.h      strftime.c  time.tex    tzset_r.c)

add_custom_command(TARGET time POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
            COMMAND llvm-ar r ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a
                    $(time_OBJECTS))
