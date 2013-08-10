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

#ifndef __FREQUENCY_H__
#define __FREQUENCY_H__

#include <time.h>
#include <uthash.h>

#include "libavl/avl.h"
#include "logtop.h"

typedef struct     s_log_line
{
    char           *string;
    char           *repr;
    unsigned int   count;
    double         frequency; /* Not live, updated lazily in logto_get */
    UT_hash_handle hh;
}                  log_line_t;

struct logtop_state
{
    double       timespan;
    unsigned int count;
    double       frequency;
    log_line_t   **lines;
};

typedef struct s_history_element
{
    log_line_t *log_entry;
    time_t     time;
}              history_element_t;

struct        logtop
{
    log_line_t        *strings;
    struct avl_table  *top;
    history_element_t *history;
    unsigned int      history_start;
    unsigned int      history_size;
};

history_element_t *new_history(struct logtop *this);
void delete_history(struct logtop *this);
history_element_t *history_oldest_element(struct logtop *this);
history_element_t *history_newest_element(struct logtop *this);
unsigned int history_length(struct logtop *this);
void history_update(struct logtop *this, log_line_t *element);

struct avl_table *new_avl(struct logtop *this);
void delete_avl(struct logtop *this);
log_line_t *avl_get(struct logtop *this, char *);
void avl_increment(struct logtop *this, log_line_t *);
void avl_decrement(struct logtop *this, log_line_t *);
void avl_traverse(struct logtop *this, unsigned int length,
                  void (*visitor)(void *data, int index, void *user_data),
                  void *user_data);

struct logtop *new_logtop(size_t history_size);
void delete_logtop(struct logtop *this);
void logtop_feed(struct logtop *this, char *line);
struct logtop_state *logtop_get(struct logtop *this, size_t qte);
void delete_logtop_state(struct logtop_state *this);
double logtop_timespan(struct logtop *this);
unsigned int logtop_qte_of_elements(struct logtop *this);

#endif
