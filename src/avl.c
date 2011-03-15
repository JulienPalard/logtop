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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "logtop.h"

static int compare_log_lines_string(const void *log_line1,
                                    const void *log_line2,
    void *avl_param)
{
    (void) avl_param;
    return (strcmp(((log_line_t*)log_line1)->string,
                   ((log_line_t*)log_line2)->string));
}

static int compare_log_lines_count(const void *log_line1,
                                   const void *log_line2,
    void *avl_param)
{
    (void) avl_param;
    if (((log_line_t*)log_line1)->count == ((log_line_t*)log_line2)->count)
        return compare_log_lines_string(log_line1, log_line2, NULL);
    return (((log_line_t*)log_line2)->count - ((log_line_t*)log_line1)->count);
}

static void die()
{
    fputs("Ran out of memory, commit suicide for important tasks to live !",
          stderr);
    exit(EXIT_FAILURE);
}

static char *repr(const char *str)
{
    char *clean;
    int  i;

    clean = strdup(str);
    if (clean == NULL)
      return NULL;
    for (i = 0; clean[i] != '\0'; ++i)
        if (clean[i] < ' ' || clean[i] > '~')
	    clean[i] = '.';
    return clean;
}

static log_line_t *create_log_entry(char *string)
{
    log_line_t *entry;

    entry = (log_line_t*)malloc(sizeof(*entry));
    if (entry == NULL)
        die();
    entry->count = 0;
    entry->string = strdup(string);
    if (entry->string == NULL)
        die();
    entry->repr = repr(string);
    if (entry->repr == NULL)
        die();
    avl_insert(gl_env.strings, entry);
    avl_insert(gl_env.top, entry);
    return entry;
}

static void delete_log_entry(log_line_t *log_entry)
{
    log_line_t *deleted;

    avl_delete(gl_env.top, log_entry);
    deleted = (log_line_t *)avl_delete(gl_env.strings, log_entry);
    free(deleted->string);
    free(deleted->repr);
    free(deleted);
}

/*
** The entry count have changed :
** The only solution to update a binary tree is to unlink and reinsert
** the entry in the tree.
*/
static void update_log_entry(log_line_t *log_entry)
{
    avl_delete(gl_env.top, log_entry);
    avl_insert(gl_env.top, log_entry);
}

void    init_avl()
{
    gl_env.history_start = 0;
    gl_env.last_update_time = time(NULL);
    gl_env.strings = avl_create(compare_log_lines_string, NULL, NULL);
    gl_env.top = avl_create(compare_log_lines_count, NULL, NULL);
    if (gl_env.strings == NULL || gl_env.top == NULL)
    {
        fputs("Not enough memory to create storage", stderr);
        exit(EXIT_FAILURE);
    }
}

log_line_t *get_log_entry(char *string)
{
    log_line_t *node;
    log_line_t search;

    search.string = string;
    node = avl_find(gl_env.strings, &search);
    if (node != NULL)
        return node;
    else
        return create_log_entry(string);
}

void increment_log_entry_count(log_line_t *log_entry)
{
    log_entry->count += 1;
    update_log_entry(log_entry);
}

void decrement_log_entry_count(log_line_t *log_entry)
{
    log_entry->count -= 1;
    if (log_entry->count == 0)
        delete_log_entry(log_entry);
    else
        update_log_entry(log_entry);
}

void traverse_log_lines(struct avl_table *tree, unsigned int length,
              void (*visitor)(void *data, int index, void *user_data),
              void *user_data)
{
    struct avl_traverser trav;
    void                 *node;
    unsigned int         last;

    last = length;
    node = avl_t_first(&trav, tree);
    while (length-- > 0)
    {
        visitor(node, last - length, user_data);
        node = avl_t_next(&trav);
    }
}
