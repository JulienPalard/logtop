/*
 * Copyright (c) 2010 Julien Palard.  All rights reserved.
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
#include <ncurses.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "logtop.h"

t_env	gl_env;

void			update_display()
{
    avl_node_t		*node;
    int			i;

    i = 0;
    clear();
    for(node = gl_env.top->head; node; node = node->next)
	mvprintw(i++, 0, "%d\t%s", ((log_entry_t*)node->item)->count, ((log_entry_t*)node->item)->string);
    refresh();
}

void			update_history(log_entry_t *element)
{
    log_entry_t		*history_element;

    history_element = gl_env.history[gl_env.history_index];
    if (history_element != NULL)
    {
	history_element->count--;
	if (history_element->count == 0)
	{
	    avl_delete_node(gl_env.top, history_element->top_node);
	    avl_delete_node(gl_env.strings, history_element->string_node);
	}
	else
	{
	    update_log_entry(history_element);
	}
    }
    gl_env.history[gl_env.history_index] = element;
    gl_env.history_index += 1;
    if (gl_env.history_index >= gl_env.history_size)
	gl_env.history_index = 0;
}

void		run()
{
    char	*string;
    size_t	size;
    log_entry_t	*element;

    string = (char*)malloc(BUFFER_SIZE);
    size = BUFFER_SIZE;
    while (1)
    {
	if (getline(&string, &size, stdin) == -1)
	{
	    perror("getline");
	    exit(EXIT_FAILURE);
	}
	element = get_log_entry(string);
	element->count += 1;
	update_log_entry(element);
	update_history(element);
	update_display();
    }
}

int	main(UNUSED(int ac), UNUSED(char **av))
{
    int	size;
    int	opt;

    size = 100;
    while ((opt = getopt(ac, av, "s:")) != -1)
    {
	switch (opt)
	{
	case 's':
	    size = atoi(optarg);
	    break;
	default:
	    fprintf(stderr, "Usage: tail something | %s [-s history_size]\n",
		    av[0]);
	    exit(EXIT_FAILURE);
	}
    }
    init_data_structures(size);
    initscr();
    curs_set(0);
    run();
    return (EXIT_SUCCESS);
}
