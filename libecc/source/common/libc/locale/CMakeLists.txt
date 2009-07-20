add_library(locale # SHARED
    fix_grouping.c  ldpart.c  lmessages.c  lmonetary.c  lnumeric.c  locale.c
    nl_langinfo.c  timelocal.c)

add_custom_command(TARGET locale POST_BUILD
    COMMAND llvm-ar r ${LIBECC_DIR}/../../lib/libecc.a $(locale_OBJECTS))
#install_targets(/lib basic)
