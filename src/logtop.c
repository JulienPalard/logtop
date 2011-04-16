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
#include "history.h"
#include "avl.h"

env_t gl_env;

void           got_a_new_string(char *string)
{
    log_line_t *element;

    element = get_log_entry(string);
    increment_log_entry_count(element);
    update_history(element);
    curses_update();
}

void        run()
{
    char    *string;
    size_t  size;
    ssize_t str_length;

    string = NULL;
    size = 0;
    while ((str_length = getline(&string, &size, stdin)) != -1)
    {
        while (str_length > 0 && (string[str_length - 1] == '\n'
                                  || string[str_length - 1] == '\r'))
        {
            string[str_length - 1] = '\0';
            str_length -= 1;
        }
        got_a_new_string(string);
    }
    if (string != NULL)
        free(string);
}

void    usage_and_exit(char* program_name)
{
    fprintf(stderr,
            "Usage: tail something | %s [-s history_size]\n"
            "    history_size: Number of log line to keep in memory\n"
            "                  Defaults to " STRINGIFY(DEFAULT_HISTORY_SIZE)
	    " lines.\n",
	    program_name);
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
    while ((opt = getopt(ac, av, "hvs:")) != -1)
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
        gl_env.history_size = DEFAULT_HISTORY_SIZE;
}

int main(int ac, char **av)
{
    parse_args(ac, av);
    init_history();
    init_avl();
    curses_setup();
    run();
    curses_release();
    stdout_update();
    return (EXIT_SUCCESS);
}
