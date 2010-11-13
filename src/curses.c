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
    double              duration;
    log_entry_t         *log_entry;
    history_element_t   *oldest_element;
    history_element_t   *newest_element;

    duration = 0;
    oldest_element = oldest_element_in_history();
    newest_element = newest_element_in_history();
    if (oldest_element != NULL && newest_element != NULL)
        duration = difftime(newest_element->time, oldest_element->time);
    i = 0;
    current_time = time(NULL);
    if (current_time == gl_env.last_update_time)
    {
        return;
    }
    gl_env.last_update_time = current_time;
    i = 0;
    clear();
    mvprintw(i++, 0, "%d elements in %d seconds\n",
           qte_of_elements_in_history(),
           (unsigned int)duration);
    for (node = gl_env.top->head;
         node != NULL && i < gl_env.display_height;
         node = node->next)
    {
        log_entry = (log_entry_t*)node->item;
        if (duration > 0)
        {
            mvprintw(i++, 0, "%d %.2f/s\t%s",
                     log_entry->count,
                     log_entry->count / (double)duration,
                     log_entry->string);
        }
        else
        {
            mvprintw(i++, 0, "%d\t%s", log_entry->count, log_entry->string);
        }
    }
    refresh();
}
