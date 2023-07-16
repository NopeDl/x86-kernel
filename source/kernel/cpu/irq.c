#include "cpu/irq.h"
#include "cpu/cpu.h"
#include "comm/cpu_instr.h"
#include "os_cfg.h"

#define IDT_TABLE_NR 128

static gate_desc_t idt_table[IDT_TABLE_NR];

static void do_default_handle(excption_frame_t *frame, const char *msg)
{
    while (1)
    {
        /* code */
    }
}

void do_handle_unknown(excption_frame_t *frame)
{
    do_default_handle(frame, "unknown exception");
}

void do_handle_divider(excption_frame_t *frame)
{
    do_default_handle(frame, "divider exception");
}

void irq_init(void)
{
    for (int i = 0; i < IDT_TABLE_NR; i++)
    {
        gate_desc_set(idt_table + i, KERNEL_SELECTOR_CS, (uint32_t)exception_handle_unknown,
                      GATE_P_PRESENT | GATE_DPL0 | GATE_TYPE_INT);
    }

    irq_install(IRQ0_DE, exception_handle_divider);

    lidt((uint32_t)idt_table, sizeof(idt_table));
}

void irq_install(int num, exception_handle_t handle)
{
    if (num >= IDT_TABLE_NR)
    {
        return;
    }
    
    gate_desc_set(idt_table + num, KERNEL_SELECTOR_CS, (uint32_t)handle,
                      GATE_P_PRESENT | GATE_DPL0 | GATE_TYPE_INT);
}
