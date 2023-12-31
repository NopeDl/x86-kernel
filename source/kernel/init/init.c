#include "comm/boot_info.h"
#include "comm/cpu_instr.h"
#include "core/memory.h"
#include "core/task.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/timer.h"
#include "ipc/sem.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "tools/log.h"
#include "dev/concole.h"

static boot_info_t* init_boot_info;

/**
 * 内核入口
 */
void kernel_init(boot_info_t* boot_info)
{
    ASSERT(boot_info->ram_region_count != 0);
    init_boot_info = boot_info;

    log_init();
    cpu_init();
    console_init();
    
    memory_init(boot_info);
    irq_init();
    timer_init();
    task_manager_init();
    // 结束此函数后会走汇编重新加载gdt
}

void move_to_first_task()
{
    task_t* cur = get_task_cur();
    ASSERT(cur != 0);

    tss_t* tss = &(cur->tss);
    __asm__ __volatile__(
        "push %[ss]\n\t"
        "push %[esp]\n\t"
        "push %[eflags]\n\t"
        "push %[cs]\n\t"
        "push %[eip]\n\t"
        "iret" ::[ss] "r"(tss->ss),
        [esp] "r"(tss->esp), [eflags] "r"(tss->eflags), [cs] "r"(tss->cs), [eip] "r"(tss->eip));
}

void init_main()
{
    log_printf("running kernel....");
    log_printf("Version: %s", OS_VERSION);

    // 初始化任务
    task_first_init();
    move_to_first_task();
}
