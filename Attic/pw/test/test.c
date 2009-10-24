// Add a comment.
#include "test.h"
#include "test.h"
#define FEE 5
/*
 * multiline
 *
 * 
 * */
#if 0

#elif 1

#if 0

#else

#endif

#elif 0

#else

#endif
#define add(a, b) \
   ((a)+ \
   (b))

#define FOOD FOOD
   FOOD
//
int main(int argc, char** argv)
{
  int i = FOO;
  i += add(FOO, FEE);
}

# define __REDIRECT(name, proto, alias) name proto __asm__ (__ASMNAME (#alias))
# define __ASMNAME(cname)  __ASMNAME2 (__USER_LABEL_PREFIX__, cname)
# define __ASMNAME2(prefix, cname) __STRING (prefix) cname
#define __STRING(x)     #x

