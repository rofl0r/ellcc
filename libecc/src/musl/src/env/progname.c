/* This constructor is pulled in if program_invocation_name or
 * program_invocation_short_name, are referenced. It grabs the
 * program name from argv[0].
 */

#include <string.h>

char *program_invocation_name, *program_invocation_short_name;

static void progname(int argc, char **argv, char **env)
    __attribute__((__constructor__, __used__));

static void progname(int argc, char **argv, char **env)
{
    char *p;

    program_invocation_name = argv[0];
    p = strrchr(argv[0], '/');
    if (p) {
        program_invocation_short_name = p + 1;
    } else {
        program_invocation_short_name = argv[0];
    }
}
