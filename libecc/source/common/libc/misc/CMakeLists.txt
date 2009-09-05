add_library(misc
    __dprintf.c  ffs.c  init.c  unctrl.c)

add_custom_command(TARGET misc POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
            COMMAND ${TARGET_AR} ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a
                    $(misc_OBJECTS))
