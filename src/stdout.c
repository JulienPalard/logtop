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

#include <stdio.h>
#include "logtop.h"

void                    stdout_update()
{
    avl_node_t          *node;
    unsigned int        i;
    history_element_t   *oldest_element;
    history_element_t   *newest_element;
    double              duration;
    log_line         *log_entry;
    unsigned int        qte_of_elements;

    duration = 0;
    oldest_element = oldest_element_in_history();
    newest_element = newest_element_in_history();
    if (oldest_element != NULL && newest_element != NULL)
        duration = difftime(newest_element->time, oldest_element->time);
    i = 0;
    qte_of_elements = qte_of_elements_in_history();
    if (duration > 0)
        printf("%d elements in %d seconds (%.2f elements/s)\n",
               qte_of_elements,
               (unsigned int)duration,
               qte_of_elements / (double)duration);
    else
        printf("%d elements in %d seconds\n",
               qte_of_elements,
               (unsigned int)duration);
    printf("%d elements in %d seconds\n",
           qte_of_elements_in_history(),
           (unsigned int)duration);
    for (node = gl_env.top->head;
         node != NULL && i < gl_env.display_height;
         node = node->next)
    {
        log_entry = (log_line*)node->item;
        if (duration > 0)
        {
            printf("%d %.2f/s\t%s",
                   log_entry->count,
                   log_entry->count / (double)duration,
                   log_entry->string);
        }
        else
        {
            printf("%d\t%s",
                   log_entry->count,
                   log_entry->string);
        }
        i += 1;
    }
}
