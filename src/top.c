#include <stdlib.h>
#include "top.h"

/* void top_dump(top_t *top) */
/* { */
/*     unsigned int i; */

/*     for (i = 0; i < top->count; ++i) */
/*         printf("[%i=%i] %s %i\n", i, top->items[i]->top_position, top->items[i]->repr, top->items[i]->count); */
/* } */


static void top_swap(top_t *top, log_line_t *left, log_line_t *right)
{
    top->items[left->top_position] = right;
    top->items[right->top_position] = left;
    left->top_position += 1;
    right->top_position -= 1;
}

top_t *top_init(size_t count)
{
    top_t *top;

    top = malloc(sizeof(*top));
    if (top == NULL)
        return NULL;
    top->count = 0;
    top->items = malloc(sizeof(*top->items) * count);
    if (top->items == NULL)
        return NULL;
    return top;
}

void top_insert(top_t *top, log_line_t *line)
{
    top->items[top->count] = line;
    line->top_position = top->count;
    top->count += 1;
}

void top_increment(top_t *top, log_line_t *line)
{
    line->count += 1;
    while (line->top_position > 0
           && top->items[line->top_position - 1]->count < line->count)
        top_swap(top, top->items[line->top_position - 1], line);
}

void top_decrement(top_t *top, log_line_t *line)
{
    line->count -= 1;
    while (line->top_position < top->count - 1
           && top->items[line->top_position + 1]->count > line->count)
        top_swap(top, line, top->items[line->top_position + 1]);
    if (line->count == 0)
        top->count -= 1;
}
