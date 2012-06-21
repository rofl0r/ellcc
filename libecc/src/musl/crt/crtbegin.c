/*-
 * Copyright 1996, 1997, 1998, 2000 John D. Polstra.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD: src/lib/csu/common/crtbegin.c,v 1.8 2002/01/28 19:02:34 obrien Exp $
 */

typedef void (*cfptr)(int argc, char **argv, char **envp);
typedef void (*dfptr)(void);

static cfptr ctor_list[1] __attribute__((section(".ctors"))) = { (cfptr) -1 };
static dfptr dtor_list[1] __attribute__((section(".dtors"))) = { (dfptr) -1 };

static void
do_ctors(int argc, char **argv, char **envp)
{
    cfptr *fpp;

    for(fpp = ctor_list + 1;  *fpp != 0;  ++fpp)
        ;
    while(--fpp > ctor_list)
        (**fpp)(argc, argv, envp);
}

static void
do_dtors(void)
{
    dfptr *fpp;

    for(fpp = dtor_list + 1;  *fpp != 0;  ++fpp)
        (**fpp)();
}

/*
 * With very large programs on some architectures (e.g., the Alpha),
 * it is possible to get relocation overflows on the limited
 * displacements of call/bsr instructions.  It is particularly likely
 * for the calls from _init() and _fini(), because they are in
 * separate sections.  Avoid the problem by forcing indirect calls.
 */
static cfptr p_do_ctors = do_ctors;
static dfptr p_do_dtors = do_dtors;

extern void _init(int argc, char **argv, char **envp) __attribute__((section(".init")));

extern void _fini(void) __attribute__((section(".fini")));

void
_fini(void)
{
    (*p_do_dtors)();
}

void
_init(int argc, char **argv, char **envp)
{
    (*p_do_ctors)(argc, argv, envp);
}

