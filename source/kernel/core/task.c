#include "core/task.h"
#include "tools/klib.h"
#include "os_cfg.h"
#include "tools/log.h"
#include "cpu/cpu.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "os_cfg.h"

static task_manager_t task_manager;

static uint32_t idle_task_stack[IDLE_STACK_SIZE];

static void idle_task_entry()
{
    while (1)
    {
        hlt();
    }
}

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

    task->time_slice = TASK_TIME_SLICE_DEFAULT;
    task->slice_ticks = TASK_TIME_SLICE_DEFAULT;
    task->sleep_ticks = 0;

    irq_state state = irq_enter_protection();
    task_set_ready(task);
    list_insert_last(&task_manager.task_list, &task->all_node);
    irq_leave_protection(state);
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
    list_init(&task_manager.sleep_list);
    task_manager.cur_task = (task_t *)0;
    task_init(&task_manager.idle_task,
              "idle-task",
              (uint32_t)idle_task_entry,
              (uint32_t)(idle_task_stack + IDLE_STACK_SIZE));
}

void task_set_ready(task_t *task)
{
    if (task == &task_manager.idle_task)
    {
        return;
    }

    list_insert_last(&task_manager.ready_list, &task->run_node);
    task->state = TASK_READY;
}

void task_set_block(task_t *task)
{
    if (task == &task_manager.idle_task)
    {
        return;
    }

    list_remove(&task_manager.ready_list, &task->run_node);
    task->state = TASK_READY;
}

void task_set_sleep(task_t *task, uint32_t ticks)
{
    if (ticks <= 0)
    {
        return;
    }

    task->sleep_ticks = ticks;
    task->state = TASK_SLEEP;
    list_insert_last(&task_manager.sleep_list, &task->run_node);
}

void task_set_wakeup(task_t *task)
{
    list_remove(&task_manager.sleep_list, &task->run_node);
}

task_t *get_task_next_run()
{
    if (list_count(&task_manager.ready_list) <= 0)
    {
        return &task_manager.idle_task;
    }

    list_node_t *task_node = list_first(&task_manager.ready_list);
    return list_node_parent(task_node, task_t, run_node);
}

task_t *get_task_cur()
{
    return task_manager.cur_task;
}

int sys_sched_yield()
{
    irq_state state = irq_enter_protection();
    if (list_count(&task_manager.ready_list) > 1)
    {
        task_t *cur_task = get_task_cur();
        task_set_block(cur_task);
        task_set_ready(cur_task);

        task_dispatch();
    }
    irq_leave_protection(state);
    return 0;
}

void task_dispatch(void)
{
    task_t *to = get_task_next_run();
    if (to != task_manager.cur_task)
    {
        task_t *from = task_manager.cur_task;
        task_manager.cur_task = to;

        to->state = TASK_RUNNING;
        task_switch_from_to(from, to);
    }
}

void sys_msleep(uint32_t ms)
{
    // 至少延时1个tick
    if (ms < OS_TICK_MS)
    {
        ms = OS_TICK_MS;
    }

    irq_state state = irq_enter_protection();

    // 从就绪队列移除，加入睡眠队列
    task_set_block(task_manager.cur_task);
    task_set_sleep(task_manager.cur_task, (ms + (OS_TICK_MS - 1)) / OS_TICK_MS);

    // 进行一次调度
    task_dispatch();

    irq_leave_protection(state);
}

/**
 * 定时中断
 */
void task_time_tick(void)
{
    task_t *curr_task = get_task_cur();

    // 时间片的处理
    irq_state state = irq_enter_protection();
    if (--curr_task->slice_ticks == 0)
    {
        // 时间片用完，重新加载时间片
        // 对于空闲任务，此处减未用
        curr_task->slice_ticks = curr_task->time_slice;

        // 调整队列的位置到尾部，不用直接操作队列
        task_set_block(curr_task);
        task_set_ready(curr_task);
    }

    // 睡眠处理
    list_node_t *curr = list_first(&task_manager.sleep_list);
    while (curr)
    {
        list_node_t *next = list_node_next(curr);

        task_t *task = list_node_parent(curr, task_t, run_node);
        if (--task->sleep_ticks == 0)
        {
            // 延时时间到达，从睡眠队列中移除，送至就绪队列
            task_set_wakeup(task);
            task_set_ready(task);
        }
        curr = next;
    }

    task_dispatch();
    irq_leave_protection(state);
}