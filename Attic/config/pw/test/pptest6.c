#pragma PFX debugoutput "pptest6.debug"
#pragma PFX preprocess on                          // Set to "on" for preprocessing only. (Same as -E)
#pragma PFX debug 0x0000000A

#define OBJ_LIKE      (1-1)
#define OBJ_LIKE      /* white space */ (1-1) /* other */
#define FUNC_LIKE(a)   ( a )
#define FUNC_LIKE( a )( /* note the white space */ \
                        a /* other stuff on this line
                           */ )

#define OBJ_LIKE     (0)     // different token sequence
#define OBJ_LIKE     (1 - 1) // different white space
#define FUNC_LIKE(b) ( b )   // different parameter spelling
#define FUNC_LIKE(b) ( a )   // different parameter usage
#define FUNC_LIKE(a, b) ( a )// different number of parameters
