#include "tools/log.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "ipc/mutex.h"
#include "tools/klib.h"
#include <stdarg.h>

static mutex_t mutex;

// 暂时使用串行接口方法
void log_init()
{
    // 清除串行接口自身中断
    outb(COM1_PORT + 1, 0x00);
    // 配置
    outb(COM1_PORT + 3, 0x80);
    outb(COM1_PORT + 0, 0x03);
    outb(COM1_PORT + 1, 0x00);
    outb(COM1_PORT + 3, 0x03);
    outb(COM1_PORT + 2, 0xc7);
    outb(COM1_PORT + 4, 0x0f);

    mutex_init(&mutex);
}

void log_printf(const char* msg, ...)
{
    char str_buf[128];
    va_list args;

    kernel_memset(str_buf, '\0', sizeof(str_buf));
    va_start(args, msg);
    kernel_vsprintf(str_buf, msg, args);
    va_end(args);

    mutex_lock(&mutex);
    const char* p = str_buf;
    while (*p != '\0') {
        // 判断是否在忙
        while ((inb(COM1_PORT + 5) & (1 << 6)) == 0)
            ;
        // 不忙了
        outb(COM1_PORT, *p++);
    }
    outb(COM1_PORT, '\r');
    outb(COM1_PORT, '\n');
    mutex_unlock(&mutex);
}