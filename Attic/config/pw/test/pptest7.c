#pragma PFX debugoutput "pptest7.debug"
#pragma PFX preprocess on                          // Set to "on" for preprocessing only. (Same as -E)
#pragma PFX debug 0x0000000A

#define debug(...)      fprintf(stderr, __VA_ARGS__)
#define showlist(...)   puts(#__VA_ARGS__)
#define report(test, ...) ((test)?puts(#test):\
            printf(__VA_ARGS__))
debug("Flag");
debug("X = %d\n", x);
showlist(The first, second, and third items.);
report(x>y, "x is %d but y is %d", x, y);
