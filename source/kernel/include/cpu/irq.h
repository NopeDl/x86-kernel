#ifndef IRQ_H
#define IRQ_H
#include "comm/types.h"

#define IRQ0_DE     0

typedef struct 
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t num, code;
    uint32_t eip, cs, eflags;
}excption_frame_t;

typedef void (*exception_handle_t)(void);

void irq_init();
void irq_install(int num, exception_handle_t handle);
void exception_handle_unknown(void);
void exception_handle_divider(void);



#endif