#include <stdio.h>
#include "logtop.h"

/*
** gl_env.history is a rotating array, so DURING THE ROTATION
** the begin of the array is the element at the gl_env.history_start
** but before, the first element is the element at index 0 and
** the element at index history_start is empty !
*/
void                    stdout_update()
{
    avl_node_t          *node;
    unsigned int        i;
    history_element_t   *oldest_element;
    history_element_t   *newest_element;
    double              duration;
    log_entry_t         *log_entry;

    duration = 0;
    oldest_element = oldest_element_in_history();
    newest_element = newest_element_in_history();
    if (oldest_element != NULL && newest_element != NULL)
        duration = difftime(newest_element->time, oldest_element->time);
    i = 0;
    printf("%d elements in %d seconds\n",
           qte_of_elements_in_history(),
           (unsigned int)duration);
    for (node = gl_env.top->head;
         node != NULL && i < gl_env.display_height;
         node = node->next)
    {
        log_entry = (log_entry_t*)node->item;
        if (duration > 0)
        {
            printf("%d %.2f/s\t%s",
                   log_entry->count,
                   log_entry->count / (double)duration,
                   log_entry->string);
        }
        else
        {
            printf("%d\t%s",
                   log_entry->count,
                   log_entry->string);
        }
    }
}
