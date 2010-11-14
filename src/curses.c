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

#include <ncurses.h>
#include <curses.h>
#include "logtop.h"

void    curses_setup()
{
    initscr();
    curs_set(0);
}

void    curses_release()
{
    endwin();
}

void                    curses_update()
{
    avl_node_t          *node;
    unsigned int        i;
    time_t              current_time;
    double              duration;
    log_line         *log_entry;
    history_element_t   *oldest_element;
    history_element_t   *newest_element;
    unsigned int        qte_of_elements;

    duration = 0;
    oldest_element = oldest_element_in_history();
    newest_element = newest_element_in_history();
    if (oldest_element != NULL && newest_element != NULL)
        duration = difftime(newest_element->time, oldest_element->time);
    i = 0;
    current_time = time(NULL);
    if (current_time == gl_env.last_update_time)
    {
        return;
    }
    gl_env.last_update_time = current_time;
    i = 0;
    qte_of_elements = qte_of_elements_in_history();
    clear();
    if (duration > 0)
        mvprintw(i++, 0, "%d elements in %d seconds (%.2f elements/s)\n",
                 qte_of_elements,
                 (unsigned int)duration,
                 qte_of_elements / (double)duration);
    else
        mvprintw(i++, 0, "%d elements in %d seconds\n",
                 qte_of_elements,
                 (unsigned int)duration);
    for (node = gl_env.top->head;
         node != NULL && i < gl_env.display_height;
         node = node->next)
    {
        log_entry = (log_line*)node->item;
        if (duration > 0)
        {
            mvprintw(i++, 0, "%d %.2f/s\t%s",
                     log_entry->count,
                     log_entry->count / (double)duration,
                     log_entry->string);
        }
        else
        {
            mvprintw(i++, 0, "%d\t%s", log_entry->count, log_entry->string);
        }
    }
    refresh();
}
