#pragma PFX debugoutput "pptest5.debug"
#pragma PFX preprocess on                          // Set to "on" for preprocessing only. (Same as -E)
#pragma PFX debug 0x0000000A

#define t(x,y,z) x ## y ## z
int j[] = { t(1,2,3), t(,4,5), t(6,,7), t(8,9,),
           t(10,,), t(,11,), t(,,12), t(,,) };
