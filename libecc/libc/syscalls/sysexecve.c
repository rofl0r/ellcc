/* connector for execve */

#include <reent.h>

int execve( char *name, char **argv, char **env)
{
  return _execve_r(_REENT, name, argv, env);
}
