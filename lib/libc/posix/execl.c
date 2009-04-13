/* execl.c */

/* This and the other exec*.c files in this directory require 
   the target to provide the _execve syscall.  */

#include <unistd.h>

/* Only deal with a pointer to environ, to work around subtle bugs with shared
   libraries and/or small data systems where the user declares his own
   'environ'.  */
static char ***p_environ = &environ;

int execl(const char *path, const char *arg1, ...)
{
  return _execve (path, (char * const  *)&arg1, *p_environ);
}
