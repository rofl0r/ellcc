add_library(locale
    fix_grouping.c  ldpart.c  lmessages.c  lmonetary.c  lnumeric.c  locale.c
    nl_langinfo.c  timelocal.c)

add_custom_command(TARGET locale POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
            COMMAND llvm-ar r ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a
                    $(locale_OBJECTS))
