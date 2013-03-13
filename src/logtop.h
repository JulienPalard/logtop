/*
 * Copyright (c) 2010 Julien Palard.  All rights reserved.
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
 */

#ifndef __LOGTOP_H__
#define __LOGTOP_H__

#include <time.h>

#include "frequency.h"

#ifndef STRINGIFY
#  define __LOGTOP_STRINGIFY(x) #x
#  define STRINGIFY(x) __LOGTOP_STRINGIFY(x)
#endif

#define DEFAULT_HISTORY_SIZE 10000

typedef struct        s_env
{
    int               quiet;
    int               line_by_line;
    unsigned int      history_size;
    unsigned int      display_width;
    unsigned int      display_height;
    time_t            last_update_time;
    time_t            interval;
    logtop            *logtop;
}                     env_t;

extern env_t gl_env;

void curses_setup(void);
void curses_release(void);
void curses_update(void);
void stdout_update(int nb_lines, int line_by_line);
void setup_sighandler(int signum, int flags, void (*act)(int));

#endif
