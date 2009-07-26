add_library(stdlib # SHARED
    a64l.c           cxa_atexit.c    gdtoa-gethex.c  malign.c      mrand48.c   srand48.c     wcstoll_r.c
    abort.c          cxa_finalize.c  gdtoa-hexnan.c  malloc.c      msize.c     strtod.c      wcstombs.c
    abs.c            div.c           getenv.c        mallocr.c     mstats.c    strtol.c      wcstombs_r.c
    __adjust.c       drand48.c       getenv_r.c      mblen.c       mtrim.c     strtoll.c     wcstoul.c
    assert.c         dtoa.c          getopt.c        mblen_r.c     nrand48.c   strtoll_r.c   wcstoull.c
    __atexit.c       dtoastub.c      getsubopt.c     mbrlen.c      on_exit.c   strtoul.c     wcstoull_r.c
    atexit.c         ecvtbuf.c       jrand48.c       mbrtowc.c     putenv.c    strtoull.c    wctob.c
    atof.c           efgcvt.c        l64a.c          mbsinit.c     putenv_r.c  strtoull_r.c  wctomb.c
    atoff.c          environ.c       labs.c          mbsrtowcs.c   rand48.c    system.c      wctomb_r.c
    atoi.c           envlock.c       lcong48.c       mbstowcs.c    rand.c      __ten_mu.c
    atol.c           eprintf.c       ldiv.c          mbstowcs_r.c  rand_r.c    valloc.c
    atoll.c          erand48.c       ldtoa.c         mbtowc.c      realloc.c   wcrtomb.c
    btowc.c          exit.c          llabs.c         mbtowc_r.c    seed48.c    wcsrtombs.c
    __call_atexit.c  _Exit.c         lldiv.c         mlock.c       setenv.c    wcstol.c
    calloc.c         __exp10.c       lrand48.c       mprec.c       setenv_r.c  wcstoll.c)

add_custom_command(TARGET stdlib POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
    COMMAND llvm-ar r ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a $(stdlib_OBJECTS))

#install_targets(/lib basic)
