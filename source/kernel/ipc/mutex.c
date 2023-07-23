#include "ipc/mutex.h"
#include "tools/list.h"
#include "cpu/irq.h"

void mutex_init(mutex_t *mutex)
{
    mutex->locked_count = 0;
    list_init(&mutex->wait_list);
    mutex->owner = (task_t *)0;
}

void mutex_lock(mutex_t *mutex)
{
    irq_state state = irq_enter_protection();
    task_t *cur = get_task_cur();
    if (mutex->locked_count == 0)
    {
        mutex->locked_count++;
        mutex->owner = cur;
    }
    else if (mutex->owner == cur)
    {
        mutex->locked_count++;
    }
    else
    {
        task_set_block(cur);
        list_insert_last(&mutex->wait_list, &cur->wait_node);
        task_dispatch();
    }

    irq_leave_protection(state);
}

void mutex_unlock(mutex_t *mutex)
{
    irq_state state = irq_enter_protection();
    task_t *cur = get_task_cur();
    if (mutex->owner == cur)
    {
        if (--mutex->locked_count == 0)
        {
            mutex->owner = (task_t *)0;
            if (list_count(&mutex->wait_list))
            {
                list_node_t* node = list_remove_first(&mutex->wait_list);
                task_t* task = list_node_parent(node, task_t, wait_node);
                task_set_ready(task);
                mutex->locked_count++;
                mutex->owner = task;
                task_dispatch();
            }
            
        }
    }

    irq_leave_protection(state);
}