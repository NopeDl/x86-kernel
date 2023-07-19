#include "tools/list.h"

void list_init(list_t *list)
{
    list->first = list->last = 0;
    list->count = 0;
}