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
				    const void *log_line2)
{
    return (strcmp(((log_line_t*)log_line1)->string,
                   ((log_line_t*)log_line2)->string));
}

static int    compare_count(const void *log_line1, const void *log_line2)
{
    if (((log_line_t*)log_line1)->count == ((log_line_t*)log_line2)->count)
    return (long)log_line1 - (long)log_line2;
    return (((log_line_t*)log_line2)->count - ((log_line_t*)log_line1)->count);
}

static void    freeitem(void *log_line)
{
    free(((log_line_t*)log_line)->string);
    free(((log_line_t*)log_line)->repr);
    free(log_line);
}

void    init_avl()
{
    gl_env.history_start = 0;
    gl_env.last_update_time = time(NULL);
    gl_env.strings = avl_alloc_tree(compare_log_lines_string, freeitem);
    gl_env.top = avl_alloc_tree(compare_count, NULL);
    if (gl_env.strings == NULL || gl_env.top == NULL)
    {
        fputs("Not enough memory to create storage", stderr);
        exit(EXIT_FAILURE);
    }
}

void die()
{
    fputs("Ran out of memory, commit suicide for important tasks to live !",
          stderr);
    exit(EXIT_FAILURE);
}

log_line_t     *create_log_entry(char *string)
{
    log_line_t *entry;
    int        i;

    entry = (log_line_t*)malloc(sizeof(*entry));
    if (entry == NULL)
        die();
    entry->count = 0;
    entry->string = strdup(string);
    if (entry->string == NULL)
        die();
    string[strlen(string) - 1] = '\0';
    for (i = 0; string[i]; ++i)
	if (string[i] < ' ' || string[i] > '~')
	    string[i] = '.';
    entry->repr = strdup(string);
    if (entry->repr == NULL)
        die();
    entry->string_node = avl_insert(gl_env.strings, entry);
    entry->top_node = avl_insert(gl_env.top, entry);
    return entry;
}

log_line_t     *get_log_entry(char *string)
{
    avl_node_t *node;
    log_line_t search;

    search.string = string;
    node = avl_search(gl_env.strings, &search);
    if (node != NULL)
        return (log_line_t*)node->item;
    else
        return create_log_entry(string);
}

/*
** The entry count have changed :
** The only solution to update the tree, is to unlink and reinsert
** the entry in the tree.
*/
static void update_log_entry(log_line_t *log_entry)
{
    avl_unlink_node(gl_env.top, log_entry->top_node);
    avl_insert_node(gl_env.top, log_entry->top_node);
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
    {
        avl_delete_node(gl_env.top, log_entry->top_node);
        avl_delete_node(gl_env.strings, log_entry->string_node);
    }
    else
    {
        update_log_entry(log_entry);
    }
}
