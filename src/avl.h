#ifndef __AVL_H__
#define __AVL_H__

#include <avl.h>

typedef struct   s_log_line
{
    char         *string;
    char         *repr;
    unsigned int count;
    avl_node_t   *string_node;
    avl_node_t   *top_node;
}                log_line_t;

void init_avl();
void increment_log_entry_count(log_line_t *);
void decrement_log_entry_count(log_line_t *);

#endif
