add_library(errno # SHARED
    errno.c)

add_custom_command(TARGET errno POST_BUILD
    COMMAND llvm-ar r ${LIBECC_DIR}/../../lib/libecc.a $(errno_OBJECTS))
#install_targets(/lib basic)
