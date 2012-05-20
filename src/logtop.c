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
#include <getopt.h>
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
    if (gl_env.quiet)
        return ;
    current_time = time(NULL);
    if (current_time < gl_env.last_update_time + gl_env.interval)
        return ;
    gl_env.last_update_time = current_time;
    if (gl_env.line_by_line)
        stdout_update(gl_env.line_by_line, 1);
    else
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

static void usage_and_exit(int exit_code)
{
    fprintf(exit_code == EXIT_SUCCESS ? stdout : stderr,
            "Usage: tail something | logtop [OPTIONS]\n"
            "    -s, --size=NUM         Number of log line to keep in memory\n"
            "                           Defaults to : "
            STRINGIFY(DEFAULT_HISTORY_SIZE) "\n"
            "    -q, --quiet            Quiet, only display a top 10 at exit.\n"
            "    -l, --line-by-line=NUM Do not use curses,"
            " do not print a top 10 at exit.\n"
            "                           Display a line by line report\n"
            "                           in a machine friendly format,\n"
            "                           NUM: quantity of result by line.\n");
    fprintf(exit_code == EXIT_SUCCESS ? stdout : stderr,
            "    -i, --interval=NUM     Interval between graphical updates,\n"
            "                           in seconds. Defaults to 1.\n"
            "\n"
            "  Line by line format is : [%%d %%f %%s\\t]*\\n\n"
            "    %%d : Number of occurences\n"
            "    %%f : Frequency of apparition\n"
            "    %%s : String (Control chars replaced by dots.\n"
            "\n");
    exit(exit_code);
}

static void version_and_exit(void)
{
    fprintf(stdout, "logtop v0.12\n");
    exit(EXIT_SUCCESS);
}

static void parse_args(int ac, char **av)
{
    int c;

    gl_env.history_size = 0;
    gl_env.quiet = 0;
    gl_env.last_update_time = 0;
    gl_env.line_by_line = 0;
    gl_env.interval = 1;
    while (1)
    {
        int option_index = 0;
        static struct option long_options[] = {
            {"size", 1, 0, 's'},
            {"quiet", 0, 0, 'q'},
            {"help", 0, 0, 'h'},
            {"version", 0, 0, 'v'},
            {"line-by-line", 1, 0, 'l'},
            {"interval", 1, 0, 'i'},
            {0, 0, 0, 0}
        };
        c = getopt_long(ac, av, "qhvl:s:i:",
                        long_options, &option_index);
        if (c == -1)
            break;
        switch (c)
        {
            case 'l':
                gl_env.line_by_line = atoi(optarg);
                break;
            case 'q':
                gl_env.quiet = 1;
                break;
            case 's':
                gl_env.history_size = atoi(optarg);
                break;
            case 'i':
                gl_env.interval = atoi(optarg);
                break;
            case 'v':
                version_and_exit();
            case 'h':
                usage_and_exit(EXIT_SUCCESS);
            default:
                usage_and_exit(EXIT_FAILURE);
        }
    }
    if (isatty(fileno(stdin)))
        usage_and_exit(EXIT_FAILURE);
    if (gl_env.history_size == 0)
        gl_env.history_size = DEFAULT_HISTORY_SIZE;
}

static void on_sigint(int sig)
{
    if (!gl_env.quiet && !gl_env.line_by_line)
        curses_release();
    setup_sighandler(SIGINT, 0, NULL);
    if (!gl_env.line_by_line)
        stdout_update(10, 0);
    fflush(NULL);
    kill(getpid(), sig);
}

int main(int ac, char **av)
{
    parse_args(ac, av);
    setup_sighandler(SIGINT, 0, on_sigint);
    init_history();
    init_avl();
    if (!gl_env.quiet && !gl_env.line_by_line)
        curses_setup();
    run();
    if (!gl_env.quiet && !gl_env.line_by_line)
        curses_release();
    if (!gl_env.line_by_line)
        stdout_update(10, 0);
    return EXIT_SUCCESS;
}
