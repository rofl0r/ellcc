add_library(crt # SHARED
    crt0.c)

add_custom_command(TARGET crt POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
    COMMAND cp $(crt_OBJECTS) ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/crt0.o)

#install_targets(/lib basic)
