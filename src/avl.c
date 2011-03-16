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

static int compare_log_lines(const void *log_line1,
                             const void *log_line2,
    void *avl_param)
{
    (void) avl_param;
    return (strcmp(((log_line_t*)log_line1)->string,
                   ((log_line_t*)log_line2)->string));
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
    top_insert(gl_env.top, entry);
    return entry;
}

void delete_log_entry(log_line_t *log_entry)
{
    log_line_t *deleted;

    deleted = (log_line_t *)avl_delete(gl_env.strings, log_entry);
    free(deleted->string);
    free(deleted->repr);
    free(deleted);
}

void init_avl()
{
    gl_env.history_start = 0;
    gl_env.last_update_time = time(NULL);
    gl_env.strings = avl_create(compare_log_lines, NULL, NULL);
    gl_env.top = calloc(gl_env.history_size, sizeof(*gl_env.top));
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

void traverse_log_lines(unsigned int length,
                        void (*visitor)(void *data, int index, void *user_data),
                        void *user_data)
{
    unsigned int i;

    for (i = 0; i < length && i < gl_env.top->count; ++i)
        visitor(gl_env.top->items[i], i, user_data);
}
