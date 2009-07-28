add_library(signal # SHARED
    signal.c)

add_custom_command(TARGET signal POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
    COMMAND llvm-ar r ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a $(signal_OBJECTS))

#install_targets(/lib basic)
