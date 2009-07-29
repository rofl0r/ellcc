add_library(elf # SHARED
    _exit.c)

add_custom_command(TARGET elf POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
    COMMAND llvm-ar r ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libtarget.a $(stdio_OBJECTS))

add_subdirectory(crt)
#install_targets(/lib basic)
