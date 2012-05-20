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
#include "history.h"

struct line_metadata
{
    double duration;
};

typedef void (*result_printer)(void *data, int index, void *metadata);

static void display_line_with_freq(void *data, int index, void *metadata)
{
    log_line_t *line;
    double     duration;

    line = (log_line_t *)data;
    duration = ((struct line_metadata *)metadata)->duration;
    printf("%4d %4d %4.2f/s %s\n",
           index,
           line->count,
           line->count / duration,
           line->repr);
}

static void display_line_without_freq(void *data, int index,
                                      void *metadata)
{
    log_line_t *line;

    (void) metadata;
    line = (log_line_t *)data;
    printf("%4d %4d %s\n",
           index,
           line->count,
           line->repr);
}

static void display_result_with_freq(void *data, int index, void *metadata)
{
    log_line_t *line;
    double     duration;

    (void) index;
    line = (log_line_t *)data;
    duration = ((struct line_metadata *)metadata)->duration;
    printf("%d %.2f %s\t",
           line->count, line->count / duration, line->repr);
}

static void display_result_without_freq(void *data, int index,
                                        void *metadata)
{
    log_line_t *line;

    (void) index;
    (void) metadata;
    line = (log_line_t *)data;
    printf("{%d, 0, %s}", line->count, line->repr);
}

static const result_printer printers[2][2] = {{display_line_without_freq,
                                               display_line_with_freq},
                                              {display_result_without_freq,
                                               display_result_with_freq}};


void stdout_update(int nb_results, int line_by_line)
{
    history_element_t    *oldest_element;
    history_element_t    *newest_element;
    struct line_metadata line_metadata;
    unsigned int         qte_of_elements;

    line_metadata.duration = 0;
    oldest_element = oldest_element_in_history();
    newest_element = newest_element_in_history();
    if (oldest_element != NULL && newest_element != NULL)
        line_metadata.duration = difftime(newest_element->time,
                                          oldest_element->time);
    qte_of_elements = qte_of_elements_in_history();
    if (!line_by_line)
    {
        if (line_metadata.duration > 0)
            printf("%d elements in %d seconds (%.2f elements/s)\n",
                   qte_of_elements,
                   (unsigned int)line_metadata.duration,
                   qte_of_elements / (double)line_metadata.duration);
        else
            printf("%d elements\n",
                   qte_of_elements);
    }
    traverse_log_lines(gl_env.top, nb_results,
                       printers[line_by_line ? 1 : 0][line_metadata.duration > 0],
                       &line_metadata);
    if (line_by_line)
        printf("\n");
}
