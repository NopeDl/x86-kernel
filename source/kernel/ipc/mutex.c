#include "ipc/mutex.h"
#include "tools/list.h"

void mutex_init(mutex_t *mutex)
{
    mutex->locked_count = 0;
    list_init(&mutex->wait_list);
    mutex->owner = (task_t *)0;
}