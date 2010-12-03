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

#include <avl.h>
#include <time.h>

#define UNUSED(x) x __attribute__((unused))

typedef struct   s_log_line
{
    char         *string;
    char         *repr;
    unsigned int count;
    avl_node_t   *string_node;
    avl_node_t   *top_node;
}                log_line;

typedef struct s_history_element
{
    log_line   *log_entry;
    time_t     time;
}              history_element_t;

typedef struct        s_env
{
    avl_tree_t        *strings;
    avl_tree_t        *top;
    history_element_t *history;
    unsigned int      history_start;
    unsigned int      history_size;
    unsigned int      display_height;
    time_t            last_update_time;
}                     env_t;

extern env_t gl_env;

/*
** In avl.c
*/

/*
** Adds or increments a string in the AVL and returns its
** representing log_line
*/
log_line *get_log_entry(char *);
void     update_log_entry(log_line *);
void     init_data_structures();

void     curses_setup();
void     curses_release();
void     curses_update();

void     stdout_update();

history_element_t *oldest_element_in_history();
history_element_t *newest_element_in_history();
unsigned int qte_of_elements_in_history();

#endif
