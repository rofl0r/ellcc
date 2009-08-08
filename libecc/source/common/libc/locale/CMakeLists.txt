add_library(locale
    fix_grouping.c  ldpart.c  lmessages.c  lmonetary.c  lnumeric.c  locale.c
    nl_langinfo.c  timelocal.c)

ELLCC_SETUP_TARGET(locale)
