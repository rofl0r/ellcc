add_library(string # SHARED
    bcmp.c     mempcpy.c     strcoll.c     strlwr.c       strsep.c    wcpncpy.c  wcsncat.c   wcwidth.c
    bcopy.c    memset.c      strcpy.c      strncasecmp.c  strspn.c    wcscat.c   wcsncmp.c   wmemchr.c
    bzero.c    rindex.c      strcspn.c     strncat.c      strstr.c    wcschr.c   wcsncpy.c   wmemcmp.c
    index.c    stpcpy.c      strdup.c      strncmp.c      strtok.c    wcscmp.c   wcsnlen.c   wmemcpy.c
    memccpy.c  stpncpy.c     strdup_r.c    strncpy.c      strtok_r.c  wcscoll.c  wcspbrk.c   wmemmove.c
    memchr.c   strcasecmp.c  strerror.c    strndup.c      strupr.c    wcscpy.c   wcsrchr.c   wmemset.c
    memcmp.c   strcasestr.c  strerror_r.c  strndup_r.c    strxfrm.c   wcscspn.c  wcsspn.c
    memcpy.c   strcat.c      strlcat.c     strnlen.c      swab.c      wcslcat.c  wcsstr.c
    memmem.c   strchr.c      strlcpy.c     strpbrk.c      u_strerr.c  wcslcpy.c  wcswidth.c
    memmove.c  strcmp.c      strlen.c      strrchr.c      wcpcpy.c    wcslen.c   wcsxfrm.c)

add_custom_command(TARGET string POST_BUILD
    COMMAND llvm-ar r ${LIBECC_DIR}/../../lib/libecc.a $(string_OBJECTS))
#install_targets(/lib basic)
