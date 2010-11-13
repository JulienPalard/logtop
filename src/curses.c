#include <ncurses.h>
#include <curses.h>
#include "logtop.h"

void    curses_setup()
{
    initscr();
    curs_set(0);
}

void    curses_release()
{
    endwin();
}

void                    curses_update()
{
    avl_node_t          *node;
    unsigned int        i;
    time_t              current_time;

    current_time = time(NULL);
    if (current_time == gl_env.last_update_time)
    {
        return;
    }
    gl_env.last_update_time = current_time;
    i = 0;
    clear();
    for (node = gl_env.top->head;
         node != NULL && i < gl_env.display_height;
         node = node->next)
        mvprintw(i++, 0, "%d\t%s", ((log_entry_t*)node->item)->count,
                 ((log_entry_t*)node->item)->string);
    refresh();
}
