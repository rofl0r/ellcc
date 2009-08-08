add_library(time
    asctime.c    ctime.c     gettzinfo.c  lcltime.c    mktime.c    strptime.c  tzlock.c   tzvars.c
    asctime_r.c  ctime_r.c   gmtime.c     lcltime_r.c  mktm_r.c    time.c      tzset.c
    clock.c      difftime.c  gmtime_r.c   local.h      strftime.c  time.tex    tzset_r.c)

ELLCC_SETUP_TARGET(time)
