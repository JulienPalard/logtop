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

/*
** Please start reading the main at the bottom of this file.
**
*/

env_t gl_env;

void                  update_history(log_entry_t *element)
{
    history_element_t *history_element;
    log_entry_t       *log_entry;

    history_element = &(gl_env.history[gl_env.history_index]);
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
    gl_env.history[gl_env.history_index].log_entry = element;
    gl_env.history_index += 1;
    if (gl_env.history_index >= gl_env.history_size)
        gl_env.history_index = 0;
}

void            got_a_new_string(char *string)
{
    log_entry_t *element;

    element = get_log_entry(string);
    element->count += 1;
    update_log_entry(element);
    update_history(element);
    curses_update();
}

void            run()
{
    char        *string;
    size_t      size;

    string = NULL;
    size = 0;
    while (getline(&string, &size, stdin) != -1)
        got_a_new_string(string);
    if (string != NULL)
        free(string);
}

void    usage_and_exit(char* program_name)
{
    fprintf(stderr, "Usage: tail something |"                           \
            " %s [-s history_size] [-c display_size]\n", program_name);
    exit(EXIT_FAILURE);
}

void    parse_args(int ac, char **av)
{
    int opt;

    gl_env.history_size = 0;
    gl_env.display_height = 0;
    while ((opt = getopt(ac, av, "s:c:")) != -1)
    {
        switch (opt)
        {
        case 's':
            gl_env.history_size = atoi(optarg);
            break;
        case 'c':
            gl_env.display_height = atoi(optarg);
            break;
        default:
            usage_and_exit(av[0]);
        }
    }
    if (gl_env.history_size == 0)
        gl_env.history_size = 100;
    if (gl_env.display_height == 0)
        gl_env.display_height = 24;
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
