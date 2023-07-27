#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/timer.h"
#include "tools/log.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "core/task.h"
#include "comm/cpu_instr.h"
#include "ipc/sem.h"
#include "core/memory.h"


static boot_info_t * init_boot_info;


/**
 * 内核入口
 */
void kernel_init(boot_info_t *boot_info)
{
    ASSERT(boot_info->ram_region_count != 0);
    init_boot_info = boot_info;

    memory_init(boot_info);
    log_init();
    cpu_init();
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
    __asm__ __volatile__ (
        "jmp *%[ip]"::[ip]"r"(tss->eip)
    );
}

void init_main()
{
    log_printf("running kernel....");
    log_printf("Version: %s", OS_VERSION);

     // 初始化任务
    task_first_init();
    move_to_first_task();
}
