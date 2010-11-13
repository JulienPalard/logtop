#include <stdio.h>
#include "logtop.h"

void                    stdout_update()
{
    avl_node_t          *node;
    unsigned int        i;

    i = 0;
    for (node = gl_env.top->head;
         node != NULL && i < gl_env.display_height;
         node = node->next)
        printf("%d\t%s", ((log_entry_t*)node->item)->count,
               ((log_entry_t*)node->item)->string);
}
