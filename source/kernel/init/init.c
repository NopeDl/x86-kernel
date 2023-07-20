#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/timer.h"
#include "tools/log.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "core/task.h"
#include "comm/cpu_instr.h"


static boot_info_t * init_boot_info;

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
    }
}

/**
 * 内核入口
 */
void kernel_init(boot_info_t *boot_info)
{
    ASSERT(boot_info->ram_region_count != 0);
    init_boot_info = boot_info;
    log_init();
    cpu_init();
    irq_init();
    timer_init();
    task_manager_init();
    // 结束此函数后会走汇编重新加载gdt
}

void init_main()
{
    log_printf("running kernel....");
    log_printf("Version: %s", OS_VERSION);
    

     // 初始化任务
    task_init(&init_task, "init-task", (uint32_t)init_task_entry, (uint32_t)&init_task_stack[1024]);
    task_first_init();
    irq_enable_global();
    int count = 0;
    for (;;) {
        log_printf("first task: %d", count++);
    }
}
