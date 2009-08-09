add_library(ctype
    ctype_.c   iscntrl.c  isspace.c   iswcntrl.c  iswprint.c   isxdigit.c  _toupper.c   wctrans.c
    isalnum.c  isdigit.c  isupper.c   iswctype.c  iswpunct.c   jp2uc.c     toupper.c    wctype.c
    isalpha.c  islower.c  iswalnum.c  iswdigit.c  iswspace.c   toascii.c   towctrans.c
    isascii.c  isprint.c  iswalpha.c  iswgraph.c  iswupper.c   _tolower.c  towlower.c
    isblank.c  ispunct.c  iswblank.c  iswlower.c  iswxdigit.c  tolower.c   towupper.c)

add_custom_command(TARGET ctype POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
            COMMAND llvm-ar r ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a
                    $(ctype_OBJECTS))
