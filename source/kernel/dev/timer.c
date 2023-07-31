#include "dev/timer.h"
#include "comm/cpu_instr.h"
#include "comm/types.h"
#include "core/task.h"
#include "cpu/irq.h"
#include "os_cfg.h"

static uint32_t sys_tick;

/**
 * 定时中断处理函数
 */
void do_handle_timer(excption_frame_t* frame)
{
    sys_tick++;
    pic_send_eoi(IRQ0_TIMER);
    task_time_tick();
}

static void init_pit()
{
    uint32_t reload_count = PIT_OSC_FREQ / (1000.0 / OS_TICK_MS);
    outb(PIT_COMMAND_MODE_PORT, PIT_CHANNEL0 | PIT_LOAD_LOHI | PIT_MODE3);
    // reload_count根据芯片信号定时减，减到0产生中断
    outb(PIT_CHANNEL0_DATA_PORT, reload_count & 0xFF); // 加载低8位
    outb(PIT_CHANNEL0_DATA_PORT, (reload_count >> 8) & 0xFF); // 再加载高8位
    irq_install(IRQ0_TIMER, exception_handle_timer);
    irq_enable(IRQ0_TIMER);
}

void timer_init()
{
    sys_tick = 0;
    init_pit();
}