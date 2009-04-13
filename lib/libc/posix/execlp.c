/* execlp.c */

/* This and the other exec*.c files in this directory require 
   the target to provide the _execve syscall.  */

#include <unistd.h>
#include <stdarg.h>

int execlp(const char *path, const char *arg1, ...)
{
  return execvp (path, (char * const *)&arg1);
}
