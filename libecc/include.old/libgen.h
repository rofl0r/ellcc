/*
 * libgen.h - defined by XPG4
 */

#ifndef _LIBGEN_H_
#define _LIBGEN_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <sys/reent.h>

char *basename(char *);
char *dirname(char *);
#ifdef __cplusplus
}
#endif

#endif

