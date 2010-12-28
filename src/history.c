#include <stdlib.h>
#include "logtop.h"

/*
** If the element under history_start is null
** then the history is not full.
*/
unsigned int qte_of_elements_in_history()
{
    if (gl_env.history[gl_env.history_start].log_entry == NULL)
        return gl_env.history_start;
    else
        return gl_env.history_size;
}

history_element_t *oldest_element_in_history()
{
    if (gl_env.history[gl_env.history_start].log_entry != NULL)
    {
        return &(gl_env.history[gl_env.history_start]);
    }
    else
    {
        if (gl_env.history_start == 0)
            return NULL;
        return &(gl_env.history[0]);
    }
}

history_element_t *newest_element_in_history()
{
    int           newest_item_index;

    newest_item_index = gl_env.history_start == 0
        ? gl_env.history_size - 1
        : gl_env.history_start - 1;
    if (gl_env.history[newest_item_index].log_entry == NULL)
        return NULL;
    else
        return &(gl_env.history[newest_item_index]);
}

void init_history()
{
    gl_env.history = (history_element_t*)calloc(sizeof(history_element_t),
						gl_env.history_size);
}
