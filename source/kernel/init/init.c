#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/timer.h"
#include "tools/log.h"
#include "os_cfg.h"

/**
 * 内核入口
 */
void kernel_init(boot_info_t *boot_info)
{
    log_init();
    cpu_init();
    irq_init();
    timer_init();
    //结束此函数后会走汇编重新加载gdt
}

void init_main()
{
    log_printf("running kernel....");
    log_printf("Version: %s", OS_VERSION);
    // irq_enable_global();
    while (1)
    {
        /* code */
    }
    
}
