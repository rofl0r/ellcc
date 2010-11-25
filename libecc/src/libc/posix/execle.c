/* execle.c */

/* This and the other exec*.c files in this directory require 
   the target to provide the _execve syscall.  */

#include <unistd.h>
#include <stdarg.h>

int execle(const char *path, const char *arg1, ...)
{
  va_list ap;
  char * const *envp;

  va_start (ap, arg1);

  /* Find the end of the argument array. */
  if (arg1 != NULL) while (va_arg(ap, const char *) != NULL) {}

  envp = va_arg(ap, char * const *);
  va_end(ap);

  return execve(path, (char * const *) &arg1, envp);
}
