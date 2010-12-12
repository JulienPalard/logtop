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

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "logtop.h"

env_t gl_env;

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

void                  update_history(log_line *element)
{
    history_element_t *history_element;
    log_line          *log_entry;

    history_element = &(gl_env.history[gl_env.history_start]);
    log_entry = history_element->log_entry;
    if (log_entry != NULL)
    {
        log_entry->count--;
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
    gl_env.history[gl_env.history_start].log_entry = element;
    gl_env.history[gl_env.history_start].time = time(NULL);
    gl_env.history_start += 1;
    if (gl_env.history_start >= gl_env.history_size)
        gl_env.history_start = 0;
}

void         got_a_new_string(char *string)
{
    log_line *element;

    element = get_log_entry(string);
    element->count += 1;
    update_log_entry(element);
    update_history(element);
    curses_update();
}

void       run()
{
    char   *string;
    size_t size;

    string = NULL;
    size = 0;
    while (getline(&string, &size, stdin) != -1)
        got_a_new_string(string);
    if (string != NULL)
        free(string);
}

void    usage_and_exit(char* program_name)
{
    fprintf(stderr, "Usage: tail something |" \
            " %s [-s history_size]\n", program_name);
    fprintf(stderr, "    history_size: Number of log line to keep\n");
    fprintf(stderr, "                  Defaults to 10000 lines.\n");
    exit(EXIT_FAILURE);
}

void    version_and_exit(char* program_name)
{
    fprintf(stderr, "%s v0.11\n", program_name);
    exit(EXIT_FAILURE);
}

void    parse_args(int ac, char **av)
{
    int opt;

    gl_env.history_size = 0;
    while ((opt = getopt(ac, av, "hvs:c:")) != -1)
    {
        switch (opt)
        {
        case 's':
            gl_env.history_size = atoi(optarg);
            break;
        case 'v':
            version_and_exit(av[0]);
            break;
        default:
            usage_and_exit(av[0]);
        }
    }
    if (gl_env.history_size == 0)
        gl_env.history_size = 10000;
}


int    main(int ac, char **av)
{
    parse_args(ac, av);
    init_data_structures();
    curses_setup();
    run();
    curses_release();
    stdout_update();
    return (EXIT_SUCCESS);
}
