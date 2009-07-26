add_library(argz # SHARED
    argz_add.c      argz_create.c      argz_insert.c     buf_findstr.c  envz_merge.c
    argz_add_sep.c  argz_create_sep.c  argz_next.c       envz_add.c     envz_remove.c
    argz_append.c   argz_delete.c      argz_replace.c    envz_entry.c   envz_strip.c
    argz_count.c    argz_extract.c     argz_stringify.c  envz_get.c)

add_custom_command(TARGET argz POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
    COMMAND llvm-ar r ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a $(argz_OBJECTS))

#install_targets(arm/lib argz)
