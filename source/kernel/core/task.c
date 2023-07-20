#include "core/task.h"
#include "tools/klib.h"
#include "os_cfg.h"
#include "tools/log.h"
#include "cpu/cpu.h"
#include "comm/cpu_instr.h"

static task_manager_t task_manager;

static int tss_init(task_t *task, uint32_t entry, uint32_t esp)
{
    int tss_selector = gdt_alloc_desc();
    if (tss_selector == -1)
    {
        log_printf("alloc tss failed....");
        return -1;
    }
    tss_t *tp = &task->tss;
    segment_desc_set(tss_selector, (uint32_t)tp, sizeof(tss_t),
                     SEG_P_PRESENT | SEG_DPL0 | SEG_TYPE_TSS);

    kernel_memset(tp, 0, sizeof(tss_t));
    tp->esp = tp->esp0 = esp;
    tp->eip = entry;
    tp->ss = tp->ss0 = KERNEL_SELECTOR_DS;
    tp->es = tp->ds = tp->fs = tp->gs = KERNEL_SELECTOR_DS;
    tp->cs = KERNEL_SELECTOR_CS;
    tp->eflags = EFLAGS_DEFAULT | EFLAGS_IF;
    task->tss_sel = tss_selector;
    return 0;
}

int task_init(task_t *task, const char *name, uint32_t entry, uint32_t esp)
{
    ASSERT(task != (task_t *)0);
    tss_init(task, entry, esp);

    list_node_init(&task->run_node);
    list_node_init(&task->all_node);

    kernel_strncpy(task->name, name, TASK_NAME_SIZE);
    task->state = TASK_CREATED;

    task->time_ticks = TASK_TIME_SLICE_DEFAULT;
    task->slice_ticks = TASK_TIME_SLICE_DEFAULT;

    task_set_ready(task);
    list_insert_last(&task_manager.task_list, &task->all_node);
    return 0;
}

void simple_switch(uint32_t **from, uint32_t *to);

void task_switch_from_to(task_t *from, task_t *to)
{
    switch_tss(to->tss_sel);
}

void task_first_init()
{
    task_init(&task_manager.first_task, "first-task", 0, 0);

    // 写TR寄存器，指示当前运行的第一个任务
    write_tr(task_manager.first_task.tss_sel);
    task_manager.cur_task = &task_manager.first_task;
}

task_t *get_first_task()
{
    return &task_manager.first_task;
}

void task_manager_init()
{
    // 各队列初始化
    list_init(&task_manager.ready_list);
    list_init(&task_manager.task_list);

    task_manager.cur_task = (task_t *)0;
}

void task_set_ready(task_t *task)
{
    list_insert_last(&task_manager.ready_list, &task->run_node);
    task->state = TASK_READY;
}

void task_set_block(task_t *task)
{
    list_remove(&task_manager.ready_list, &task->run_node);
    task->state = TASK_READY;
}

task_t *get_task_next_run()
{
    list_node_t *task_node = list_first(&task_manager.ready_list);
    return list_node_parent(task_node, task_t, run_node);
}

task_t *get_task_cur()
{
    return task_manager.cur_task;
}

int sys_sched_yield()
{
    if (list_count(&task_manager.ready_list) <= 1)
    {
        return 0;
    }
    task_t *cur_task = get_task_cur();
    task_set_block(cur_task);
    task_set_ready(cur_task);

    task_dispatch();
}

void task_dispatch()
{
    task_t *to = get_task_next_run();
    task_t *from = get_task_cur();

    if (to == from)
    {
        return;
    }
    task_manager.cur_task = to;
    to->state = TASK_RUNNING;
    task_switch_from_to(from, to);
}

/**
 * 定时中断
*/
void task_time_tick()
{
    task_t *cur_task = get_task_cur();
    if (--cur_task->slice_ticks == 0)
    {
        cur_task->slice_ticks = cur_task->time_ticks;
        task_set_block(cur_task);
        task_set_ready(cur_task);
        task_dispatch();
    }
}