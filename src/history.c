/*
 * Copyright (c) 2010 Julien Palard.
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
#include "logtop.h"

/*
** If the element under history_start is null
** then the history is not full.
*/
unsigned int qte_of_elements_in_history()
{
    if (gl_env.history[gl_env.history_start].log_entry == NULL)
        return gl_env.history_start;
    else
        return gl_env.history_size;
}

history_element_t *oldest_element_in_history()
{
    if (gl_env.history[gl_env.history_start].log_entry != NULL)
    {
        return &(gl_env.history[gl_env.history_start]);
    }
    else
    {
        if (gl_env.history_start == 0)
            return NULL;
        return &(gl_env.history[0]);
    }
}

history_element_t *newest_element_in_history()
{
    int           newest_item_index;

    newest_item_index = gl_env.history_start == 0
        ? gl_env.history_size - 1
        : gl_env.history_start - 1;
    if (gl_env.history[newest_item_index].log_entry == NULL)
        return NULL;
    else
        return &(gl_env.history[newest_item_index]);
}

void update_history(log_line_t *element)
{
    history_element_t *history_element;
    log_line_t        *log_entry;

    history_element = &(gl_env.history[gl_env.history_start]);
    log_entry = history_element->log_entry;
    if (log_entry != NULL)
        decrement_log_entry_count(log_entry);
    gl_env.history[gl_env.history_start].log_entry = element;
    gl_env.history[gl_env.history_start].time = time(NULL);
    gl_env.history_start += 1;
    if (gl_env.history_start >= gl_env.history_size)
        gl_env.history_start = 0;
}

void init_history()
{
    gl_env.history = (history_element_t*)calloc(sizeof(history_element_t),
                                                gl_env.history_size);
}
