add_library(stdio
    asiprintf.c   fgets.c        fseek.c      gets.c            refill.c      sprintf.c      viscanf.c
    asniprintf.c  fileno.c       fseeko.c     getw.c            remove.c      sscanf.c       vprintf.c
    asnprintf.c   findfp.c       fsetpos.c    iprintf.c         rename.c      stdio.c        vscanf.c
    asprintf.c    fiprintf.c     ftell.c      iscanf.c          rewind.c      tmpfile.c      vsiprintf.c
    clearerr.c    fiscanf.c      ftello.c     makebuf.c         rget.c        tmpnam.c       vsiscanf.c
    diprintf.c    flags.c        funopen.c    mktemp.c          scanf.c       ungetc.c       vsniprintf.c
    dprintf.c     fmemopen.c     fvwrite.c    open_memstream.c  sccl.c        vasiprintf.c   vsnprintf.c
    fcloseall.c   fopen.c        fwalk.c      perror.c          setbuf.c      vasniprintf.c  vsprintf.c
    fclose.c      fopencookie.c  fwrite.c     printf.c          setbuffer.c   vasnprintf.c   vsscanf.c
    fdopen.c      fprintf.c      getc.c       putc.c            setlinebuf.c  vasprintf.c    wbuf.c
    feof.c        fputc.c        getchar.c    putchar.c         setvbuf.c     vdiprintf.c    wsetup.c
    ferror.c      fputs.c        getchar_u.c  putchar_u.c       siprintf.c    vdprintf.c
    fflush.c      fread.c        getc_u.c     putc_u.c          siscanf.c     vfprintf.c
    fgetc.c       freopen.c      getdelim.c   puts.c            sniprintf.c   vfscanf.c
    fgetpos.c     fscanf.c       getline.c    putw.c            snprintf.c
    viprintf.c)

add_custom_command(TARGET stdio POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
            COMMAND llvm-ar r ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a
                    $(stdio_OBJECTS))
