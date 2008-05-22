#pragma PFX debugoutput "pptest6a.debug"
#pragma PFX preprocess on                          // Set to "on" for preprocessing only. (Same as -E)
#pragma PFX debug 0x0000000C

#include "pptest6a.h"

#define OBJ_LIKE     (0)     // different token sequence
#define OBJ_LIKE     (1 - 1) // different white space
#define FUNC_LIKE(b) ( b )   // different parameter spelling
#define FUNC_LIKE(b) ( a )   // different parameter usage
#define FUNC_LIKE(a, b) ( a )// different number of parameters
