#ifndef TASK_H
#define TASK_H
#include "comm/types.h"
#include "cpu/cpu.h"
#include "tools/list.h"

typedef struct
{
    uint32_t *stack;
    tss_t tss;
    int tss_sel;
} task_t;

int task_init(task_t *task, uint32_t entry, uint32_t esp);
void task_switch_from_to(task_t *from, task_t *to);

typedef struct _task_manager_t
{
    task_t *cur_task;
    task_t first_task;  // 第一个任务(引导程序)
    list_t *ready_list; // 就绪队列
    list_t *task_list;  // 所有进程
} task_manager_t;

void task_manager_init();
void task_first_init();
task_t *get_first_task();

#endif