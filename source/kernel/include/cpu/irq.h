#ifndef IRQ_H
#define IRQ_H
#include "comm/types.h"

typedef struct 
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t eip, cs, eflags;
}excption_frame_t;



void irq_init();


#endif