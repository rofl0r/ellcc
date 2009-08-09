add_library(search
    bsearch.c      hash_buf.c  hash_func.c  hash_page.c  hcreate_r.c  tdelete.c   tfind.c    twalk.c
    hash_bigkey.c  hash.c      hash_log2.c  hcreate.c    qsort.c      tdestroy.c  tsearch.c)

add_custom_command(TARGET search POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
            COMMAND llvm-ar r ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a
                    $(search_OBJECTS))
