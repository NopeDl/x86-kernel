#ifndef TASK_H
#define TASK_H
#include "comm/types.h"
#include "cpu/cpu.h"
#include "tools/list.h"

#define TASK_NAME_SIZE 32
#define TASK_TIME_SLICE_DEFAULT 10

#define TASK_FLAGS_SYSTEM (1 << 0)

typedef struct _task_t
{
    enum
    {
        TASK_CREATED,
        TASK_RUNNING,
        TASK_SLEEP,
        TASK_READY,
        TASK_WAITING,
    } state;

    char name[TASK_NAME_SIZE]; // 任务名字
    int pid;

    int sleep_ticks; // 睡眠时间
    int time_slice;  // 时间片
    int slice_ticks; // 递减时间片计数

    tss_t tss;        // 任务的TSS段
    uint16_t tss_sel; // tss选择子

    list_node_t run_node;  // 运行相关结点
    list_node_t wait_node; // 等待队列结点
    list_node_t all_node;  // 所有队列结点
} task_t;

int task_init(task_t *task, const char *name, int flags, uint32_t entry, uint32_t esp);
void task_switch_from_to(task_t *from, task_t *to);

typedef struct _task_manager_t
{
    task_t *cur_task;
    task_t first_task; // 第一个任务(引导程序)
    task_t idle_task;  // 空闲任务
    list_t ready_list; // 就绪队列
    list_t task_list;  // 所有进程
    list_t sleep_list; // 延时队列

    int app_code_sel;
    int app_data_sel;
} task_manager_t;

void task_manager_init();
void task_first_init();
task_t *get_first_task();

void task_set_ready(task_t *task);
void task_set_block(task_t *task);
void task_set_sleep(task_t *task, uint32_t ticks);
void task_set_wakeup(task_t *task);

task_t *get_task_next_run();
task_t *get_task_cur();

int sys_sched_yield();
void task_dispatch();
void sys_sleep(uint32_t ms);

/**
 * 定时中断处理
 */
void task_time_tick();

int sys_get_pid();

#endif