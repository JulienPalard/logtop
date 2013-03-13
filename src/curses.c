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

#define _GNU_SOURCE
#include <ncurses.h>
#include <curses.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "logtop.h"

static WINDOW *window;

static void update_winsize(void)
{
    struct winsize ws;

    if (ioctl(1, TIOCGWINSZ, &ws) == -1)
    {
        gl_env.display_height = 24;
        gl_env.display_width = 80;
    }
    else
    {
        gl_env.display_height = ws.ws_row;
        gl_env.display_width = ws.ws_col;
    }
}

static void on_sigwinch(int sig __attribute__((unused)))
{
    update_winsize();
    endwin();
    delwin(window);
    window = newwin(gl_env.display_height, gl_env.display_width, 0, 0);
    curses_update();
}

void curses_setup()
{
    update_winsize();
    setup_sighandler(SIGWINCH, SA_RESTART, on_sigwinch);
    initscr();
    window = newwin(gl_env.display_height, gl_env.display_width, 0, 0);
}

void curses_release()
{
    delwin(window);
    endwin();
}

struct display_data
{
    double       duration;
    unsigned int qte_of_elements;
};

static void display_line(void *data __attribute__((unused)),
                         int index, void *display_data)
{
    log_line_t *line;

    line = (log_line_t *)data;
    mvwprintw(window, index + 1, 0, "%4d %6d %8.2f %-*s",
              index,
              line->count,
              line->count / ((struct display_data*)display_data)->duration,
              gl_env.display_width - 21, line->repr);
}

static void display_header(struct display_data *display_data)
{
    mvwprintw(window, 0, 0,
              "%u lines, %.2f lines/s",
              display_data->qte_of_elements,
              display_data->qte_of_elements / display_data->duration);
    mvwprintw(window, 1, 0,
              "RANK    CNT   LINE/S LINE");
    mvwchgat(window, 1, 0, -1, A_REVERSE, 0, NULL);
}

void curses_update()
{
    struct display_data display_data;
    history_element_t  *oldest_element;
    history_element_t  *newest_element;

    display_data.duration = 0;
    oldest_element = oldest_element_in_history(gl_env.logtop);
    newest_element = newest_element_in_history(gl_env.logtop);
    if (oldest_element != NULL && newest_element != NULL)
        display_data.duration = difftime(newest_element->time,
                                         oldest_element->time);
    display_data.qte_of_elements = qte_of_elements_in_history(gl_env.logtop);
    werase(window);
    display_header(&display_data);
    traverse_log_lines(gl_env.logtop, gl_env.display_height - 2,
                       display_line, &display_data);
    wrefresh(window);
}
