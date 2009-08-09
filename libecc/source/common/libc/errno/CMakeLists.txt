add_library(errno
    errno.c)

add_custom_command(TARGET errno POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
            COMMAND llvm-ar r ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a
                    $(errno_OBJECTS))
