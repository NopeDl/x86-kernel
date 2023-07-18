#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/timer.h"
#include "tools/log.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "core/task.h"
#include "comm/cpu_instr.h"




static task_t first_task;       // 第一个任务
static uint32_t init_task_stack[1024];	// 空闲任务堆栈
static task_t init_task;

/**
 * 初始任务函数
 * 目前暂时用函数表示，以后将会作为加载为进程
 */
void init_task_entry(void) {
    int count = 0;

    for (;;) {
        log_printf("init task: %d", count++);
        task_switch_from_to(&init_task, &first_task);
    }
}

/**
 * 内核入口
 */
void kernel_init(boot_info_t *boot_info)
{
    ASSERT(boot_info->ram_region_count != 0);
    log_init();
    cpu_init();
    irq_init();
    timer_init();
    // 结束此函数后会走汇编重新加载gdt
}

void init_main()
{
    log_printf("running kernel....");
    log_printf("Version: %s", OS_VERSION);
    // irq_enable_global();
    

     // 初始化任务
    task_init(&init_task, (uint32_t)init_task_entry, (uint32_t)&init_task_stack[1024]);
    task_init(&first_task, 0, 0);
    write_tr(first_task.tss_sel);
    int count = 0;
    for (;;) {
        log_printf("first task: %d", count++);
        task_switch_from_to(&first_task, &init_task);
    }
}
