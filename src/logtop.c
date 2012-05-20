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
#include <signal.h>
#include "logtop.h"
#include "history.h"
#include "avl.h"

env_t gl_env;

/**
 * Basic sig handling using sigaction.
 * Reset action to SIG_DFL if act is NULL.
 */
void setup_sighandler(int signum, int flags, void (*act)(int))
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    if (act != NULL)
        sa.sa_handler = act;
    else
        sa.sa_handler = SIG_DFL;
    sa.sa_flags = flags;
    sigaction(signum, &sa, NULL);
}

static void got_a_new_string(char *string)
{
    log_line_t *element;
    time_t     current_time;

    element = get_log_entry(string);
    increment_log_entry_count(element);
    update_history(element);
    if (!gl_env.with_curses)
        return ;
    current_time = time(NULL);
    if (current_time == gl_env.last_update_time)
        return ;
    gl_env.last_update_time = current_time;
    curses_update();
}

static void run(void)
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

static void usage_and_exit(char* program_name)
{
    fprintf(stderr,
            "Usage: tail something | %s [-s history_size] [-q]\n"
            "    -s: Number of log line to keep in memory\n"
            "        Defaults to " STRINGIFY(DEFAULT_HISTORY_SIZE)
            "    -q: Quiet, only display a top 10 at exit."
            " lines.\n",
            program_name);
    exit(EXIT_FAILURE);
}

static void version_and_exit(char* program_name)
{
    fprintf(stderr, "%s v0.11\n", program_name);
    exit(EXIT_FAILURE);
}

static void parse_args(int ac, char **av)
{
    int opt;

    gl_env.history_size = 0;
    gl_env.with_curses = 1;
    while ((opt = getopt(ac, av, "hvs:q")) != -1)
    {
        switch (opt)
        {
            case 'q':
                gl_env.with_curses = 0;
                break;
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

static void on_sigint(int sig)
{
    if (gl_env.with_curses)
        curses_release();
    setup_sighandler(SIGINT, 0, NULL);
    stdout_update(10);
    fflush(NULL);
    kill(getpid(), sig);
}

int main(int ac, char **av)
{
    parse_args(ac, av);
    setup_sighandler(SIGINT, 0, on_sigint);
    init_history();
    init_avl();
    if (gl_env.with_curses)
        curses_setup();
    run();
    if (gl_env.with_curses)
        curses_release();
    stdout_update(10);
    return EXIT_SUCCESS;
}
