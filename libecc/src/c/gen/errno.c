/* The errno variable is stored in the reentrancy structure.  This
   function returns its address for use by the macro errno defined in
   errno.h.  */

#include <errno.h>

static int errno_val;

int * __errno()
{
  return &errno_val;        // RICH: Threads.
}
