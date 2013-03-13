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
#include "main.h"

struct display_data
{
    double       duration;
    unsigned int qte_of_elements;
};

typedef void (*result_printer)(void *data, int index, void *metadata);

static void display_line(void *data, int index, void *metadata)
{
    log_line_t *line;

    line = (log_line_t *)data;
    printf("%4d %6d %8.2f %-*s\n",
           index,
           line->count,
           line->count / ((struct display_data *)metadata)->duration,
           gl_env.display_width - 21, line->repr);
}

static void display_result(void *data,
                           int index __attribute__((unused)),
                           void *metadata)
{
    log_line_t *line;

    line = (log_line_t *)data;
    printf("%d %.2f %s\t",
           line->count,
           line->count / ((struct display_data *)metadata)->duration,
           line->repr);
}

static void display_header(struct display_data *display_data)
{
    printf("%d lines, %.2f lines/s\n",
           display_data->qte_of_elements,
           display_data->qte_of_elements / (double)display_data->duration);
    printf("%s\n", "RANK    CNT   LINE/S LINE");
}

static const result_printer printers[2] = {display_line,
                                           display_result};


void stdout_update(int nb_results, int line_by_line)
{
    history_element_t   *oldest_element;
    history_element_t   *newest_element;
    struct display_data display_data;

    display_data.duration = 1;
    oldest_element = history_oldest_element(gl_env.logtop);
    newest_element = history_newest_element(gl_env.logtop);
    if (oldest_element != NULL && newest_element != NULL)
        display_data.duration = difftime(newest_element->time,
                                         oldest_element->time);
    display_data.qte_of_elements = history_length(gl_env.logtop);
    if (line_by_line)
    {
        avl_traverse(gl_env.logtop, nb_results,
                           display_result,
                           &display_data);
        printf("\n");
    }
    else
    {
        display_header(&display_data);
        avl_traverse(gl_env.logtop, nb_results,
                           display_line,
                           &display_data);
    }
}
