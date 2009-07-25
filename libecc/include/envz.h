/* Copyright (C) 2002 by  Red Hat, Incorporated. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#ifndef _ENVZ_H_
#define _ENVZ_H_

_BEGIN_STD_C
#include <errno.h>
#include <sys/types.h>

char * envz_entry (const char *envz, size_t envz_len, const char *name);
char * envz_get (const char *envz, size_t envz_len, const char *name);
error_t envz_add (char **envz, size_t *envz_len, const char *name, const char *value);
error_t envz_merge (char **envz, size_t *envz_len, const char *envz2, size_t envz2_len, int override);
void envz_remove(char **envz, size_t *envz_len, const char *name);
void envz_strip (char **envz, size_t *envz_len);
_END_STD_C

#endif
