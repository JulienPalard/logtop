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
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "logtop.h"
#include "history.h"

static WINDOW *window;

static void curses_update_winsize(void)
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

static void curses_on_sigwinch(int sig)
{
    (void) sig;
    curses_update_winsize();
    endwin();
    delwin(window);
    window = newwin(gl_env.display_height, gl_env.display_width, 0, 0);
    curses_update();
}

void curses_setup()
{
    curses_update_winsize();
    setup_sighandler(SIGWINCH, SA_RESTART, curses_on_sigwinch);
    initscr();
    window = newwin(gl_env.display_height, gl_env.display_width, 0, 0);
}

void curses_release()
{
    delwin(window);
    endwin();
}

struct line_metadata
{
    double duration;
};

static void display_line_with_freq(void *data, int index, void *metadata)
{
    log_line_t *line;
    double     duration;

    line = (log_line_t *)data;
    duration = ((struct line_metadata *)metadata)->duration;
    mvwprintw(window, index, 0, "%4d %4d %8.2f/s %-*s",
              index,
              line->count,
              line->count / duration,
              gl_env.display_width - 21, line->repr);
}

static void display_line_without_freq(void *data, int index, void *metadata)
{
    log_line_t *line;

    (void) metadata;
    line = (log_line_t *)data;
    mvwprintw(window, index, 0, "%4d %4d %s",
              index,
              line->count,
              line->repr);
}

void curses_update()
{
    struct line_metadata line_data;
    history_element_t    *oldest_element;
    history_element_t    *newest_element;
    unsigned int         qte_of_elements;

    line_data.duration = 0;
    oldest_element = oldest_element_in_history();
    newest_element = newest_element_in_history();
    if (oldest_element != NULL && newest_element != NULL)
        line_data.duration = difftime(newest_element->time,
                                      oldest_element->time);
    qte_of_elements = qte_of_elements_in_history();
    werase(window);
    if (line_data.duration > 0)
        mvwprintw(window, 0, 0, "%d elements in %d seconds (%.2f elements/s)",
                  qte_of_elements,
                  (unsigned int)line_data.duration,
                  qte_of_elements / (double)line_data.duration,
                  gl_env.display_height);
    else
        mvwprintw(window, 0, 0, "%d elements in %d seconds",
                  qte_of_elements,
                  (unsigned int)line_data.duration);
    if (line_data.duration > 0)
        traverse_log_lines(gl_env.top, gl_env.display_height - 1,
                           display_line_with_freq, &line_data);
    else
        traverse_log_lines(gl_env.top, gl_env.display_height - 1,
                           display_line_without_freq, &line_data);
    wrefresh(window);
}
