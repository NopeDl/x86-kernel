#include "cpu/irq.h"
#include "cpu/cpu.h"
#include "comm/cpu_instr.h"
#include "os_cfg.h"

#define IDT_TABLE_NR 128

static gate_desc_t idt_table[IDT_TABLE_NR];

void exception_handle_unknown(void);


static void do_default_handle(excption_frame_t* frame, const char * msg)
{
    while (1)
    {
        /* code */
    }
    
}

void do_handle_unknown(excption_frame_t* frame)
{
    do_default_handle(frame, "unknown exception");
}

void irq_init(void)
{
    for (int i = 0; i < IDT_TABLE_NR; i++)
    {
        gate_desc_set(idt_table + i, KERNEL_SELECTOR_CS, (uint32_t)exception_handle_unknown,
        GATE_P_PRESENT | GATE_DPL0 | GATE_TYPE_INT);
    }

    lidt((uint32_t)idt_table, sizeof(idt_table));
}
