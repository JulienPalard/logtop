#ifndef __TOP_H__
#define __TOP_H__

#include "avl.h"

typedef struct s_top
{
    size_t     count;
    log_line_t **items;
}              top_t;

top_t *top_init(size_t count);
void top_insert(top_t *top, log_line_t *line);
void top_increment(top_t *top, log_line_t *line);
void top_decrement(top_t *top, log_line_t *line);

#endif
