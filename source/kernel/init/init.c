#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"

/**
 * 内核入口
 */
void kernel_init(boot_info_t *boot_info)
{
    cpu_init();
    irq_init();
    //结束此函数后会走汇编重新加载gdt
}

void init_main()
{
    int a = 3 / 0;
}
