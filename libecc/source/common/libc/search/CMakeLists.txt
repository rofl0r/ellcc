add_library(search
    bsearch.c      hash_buf.c  hash_func.c  hash_page.c  hcreate_r.c  tdelete.c   tfind.c    twalk.c
    hash_bigkey.c  hash.c      hash_log2.c  hcreate.c    qsort.c      tdestroy.c  tsearch.c)

ELLCC_SETUP_TARGET(search)
