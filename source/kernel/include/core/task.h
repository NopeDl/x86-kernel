#ifndef TASK_H
#define TASK_H
#include "comm/types.h"
#include "cpu/cpu.h"
#include "tools/list.h"

#define TASK_NAME_SIZE 32
#define TASK_TIME_SLICE_DEFAULT 10

typedef struct
{
    enum
    {
        TASK_CREATED,
        TASK_RUNNING,
        TASK_SLEEP,
        TASK_READY,
        TASK_WAITING
    } state;

    int slice_ticks; // 递减
    int time_ticks;

    char name[TASK_NAME_SIZE];

    list_node_t run_node;
    list_node_t all_node;
    tss_t tss;
    int tss_sel;
} task_t;

int task_init(task_t *task, const char *name, uint32_t entry, uint32_t esp);
void task_switch_from_to(task_t *from, task_t *to);

typedef struct _task_manager_t
{
    task_t *cur_task;
    task_t first_task; // 第一个任务(引导程序)
    list_t ready_list; // 就绪队列
    list_t task_list;  // 所有进程
} task_manager_t;

void task_manager_init();
void task_first_init();
task_t *get_first_task();

void task_set_ready(task_t *task);
void task_set_block(task_t *task);

task_t *get_task_next_run();
task_t *get_task_cur();

int sys_sched_yield();
void task_dispatch();

/**
 * 定时中断处理
*/
void task_time_tick();


#endif