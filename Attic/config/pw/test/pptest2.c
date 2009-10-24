#pragma PFX debugoutput "pptest2.debug"
#pragma PFX preprocess on                          // Set to "on" for preprocessing only. (Same as -E)
#pragma PFX debug 0x0000000A


#define hash_hash # ## #
#define mkstr(a) # a
#define in_between(a) mkstr(a)
#define join(c, d) in_between(c hash_hash d)

char p[] = join(x, y); // equivalent to
                       // char p[] = "x ## y";

