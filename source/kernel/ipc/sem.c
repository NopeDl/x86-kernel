#include "ipc/sem.h"
#include "core/task.h"
#include "cpu/irq.h"

void sem_init(sem_t *sem, int init_count)
{
    sem->count = init_count;
    list_init(&sem->wait_list);
}

void sem_wait(sem_t *sem)
{
    irq_state state = irq_enter_protection();
    if (sem->count > 0)
    {
        sem->count--;
    }
    else
    {
        task_t *cur = get_task_cur();
        task_set_block(cur);
        list_insert_last(&sem->wait_list, &cur->wait_node);
        task_dispatch();
    }
    irq_leave_protection(state);
}

void sem_notify(sem_t *sem)
{
    irq_state state = irq_enter_protection();

    if (list_count(&sem->wait_list))
    {
        list_node_t *node = list_remove_first(&sem->wait_list);
        task_t *task = list_node_parent(node, task_t, wait_node);
        task_set_ready(task);
        task_dispatch();
    }
    else
    {
        sem->count++;
    }

    irq_leave_protection(state);
}