add_library(signal # SHARED
    raise.c  signal.c)

add_custom_command(TARGET signal POST_BUILD
    COMMAND llvm-ar r ${LIBECC_DIR}/../../lib/libecc.a $(signal_OBJECTS))
#install_targets(/lib basic)
