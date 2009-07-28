add_library(elf # SHARED
    _exit.c)

add_custom_command(TARGET softfp POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
    COMMAND cp ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a $(softfp_OBJECTS))

#install_targets(/lib basic)
