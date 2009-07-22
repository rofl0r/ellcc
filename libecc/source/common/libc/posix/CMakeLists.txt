add_definitions(-DHAVE_OPENDIR)
add_library(posix # SHARED
    closedir.c  execle.c  execve.c   isatty.c   readdir.c    scandir.c  telldir.c
    creat.c     execlp.c  execvp.c   opendir.c  readdir_r.c  seekdir.c  usleep.c
    execl.c     execv.c   _isatty.c  popen.c    rewinddir.c  sleep.c)

#install_targets(/lib basic)
