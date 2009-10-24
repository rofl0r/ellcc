#pragma PFX preprocess on                          // Set to "on" for preprocessing only. (Same as -E)
#define VERSION 2

#if VERSION == 1
      #define INCFILE "vers1.h"
#elif VERSION == 2
      #define INCFILE "vers2.h" // and so on
#elif VERSION == 3
      #define INCFILE "vers3.h" // and so on
#else
      #define INCFILE "versN.h"
#endif
#include INCFILE
