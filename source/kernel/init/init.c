#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/timer.h"

/**
 * 内核入口
 */
void kernel_init(boot_info_t *boot_info)
{
    cpu_init();
    irq_init();
    timer_init();
    //结束此函数后会走汇编重新加载gdt
}

void init_main()
{
    irq_enable_global();
    while (1)
    {
        /* code */
    }
    
}
