add_library(libm
    frexp.c)

add_custom_command(TARGET libm POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
    COMMAND ${TARGET_AR} ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a $(libm_OBJECTS))

ELLCC_SETUP_TARGET(libm)
