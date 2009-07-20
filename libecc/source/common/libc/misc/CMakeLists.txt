add_library(misc # SHARED
    __dprintf.c  ffs.c  init.c  unctrl.c)

add_custom_command(TARGET misc POST_BUILD
    COMMAND llvm-ar r ${LIBECC_DIR}/../../lib/libecc.a $(misc_OBJECTS))
#install_targets(/lib basic)
