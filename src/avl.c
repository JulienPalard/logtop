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

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "logtop.h"


static int compare_log_lines_count(const void *log_line1,
                                   const void *log_line2,
                                   void *avl_param)
{
    (void) avl_param;
    if (((log_line_t*)log_line1)->count != ((log_line_t*)log_line2)->count)
    {
        if (((log_line_t*)log_line2)->count > ((log_line_t*)log_line1)->count)
            return 1;
        else
            return -1;
    }
    return (strcmp(((log_line_t*)log_line1)->string,
                   ((log_line_t*)log_line2)->string));

}

static void die(void)
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

static log_line_t *create_log_entry(struct logtop *this, char *string)
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
    HASH_ADD_KEYPTR(hh, this->strings, entry->string, strlen(entry->string),
                    entry);
    avl_insert(this->top, entry);
    return entry;
}

static void delete_log_entry(struct logtop *this, log_line_t *log_entry)
{
    HASH_DEL(this->strings, log_entry);
    free(log_entry->string);
    free(log_entry->repr);
    free(log_entry);
}

struct avl_table     *new_avl(struct logtop *this)
{
    return avl_create(compare_log_lines_count, this, NULL);
}

static void free_avl_element(void *avl_item, void *avl_param)
{
    delete_log_entry((struct logtop*)avl_param, (log_line_t *)avl_item);
}

void delete_avl(struct logtop *this)
{
    avl_destroy(this->top, free_avl_element);
}

log_line_t *avl_get(struct logtop *this, char *string)
{
    log_line_t *node;

    HASH_FIND_STR(this->strings, string, node);
    if (node != NULL)
        return node;
    else
        return create_log_entry(this, string);
}

void avl_increment(struct logtop *this, log_line_t *log_entry)
{
    avl_delete(this->top, log_entry);
    log_entry->count += 1;
    avl_insert(this->top, log_entry);
}

void avl_decrement(struct logtop *this, log_line_t *log_entry)
{
    avl_delete(this->top, log_entry);
    log_entry->count -= 1;
    if (log_entry->count != 0)
        avl_insert(this->top, log_entry);
    else
        delete_log_entry(this, log_entry);
}

void avl_traverse(struct logtop*logtop, unsigned int length,
                  void (*visitor)(void *data, int index, void *user_data),
                  void *user_data)
{
    struct avl_traverser trav;
    void                 *node;
    unsigned int         last;
    struct avl_table     *tree;

    tree = logtop->top;
    last = length;
    node = avl_t_first(&trav, tree);
    while (length-- > 0 && node != NULL)
    {
        visitor(node, last - length, user_data);
        node = avl_t_next(&trav);
    }
}
