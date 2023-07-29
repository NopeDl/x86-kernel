#include "cpu/cpu.h"
#include "os_cfg.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "ipc/mutex.h"
#include "core/syscall.h"

static segment_desc_t gdt_table[GDT_TABLE_SIZE];
static mutex_t mutex;

void segment_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr)
{
    segment_desc_t *desc = gdt_table + selector / sizeof(segment_desc_t);

    if (limit > 0xfffff)
    {
        limit /= 0x1000;
        attr |= 0x8000;
    }
    

    desc->limit15_0 = limit & 0xffff;
    desc->base15_0 = base & 0xffff;
    desc->base23_16 = (base >> 16) & 0xff;
    desc->attr = attr | (((limit >> 16) & 0xf) << 8);
    desc->base31_24 = (base >> 24) & 0xff;
}

void gate_desc_set(gate_desc_t* desc ,uint16_t selector, uint32_t offset, uint16_t attr)
{
    desc->attr = attr;
    desc->offset15_0 = offset & 0xffff;
    desc->offset31_16 = (offset >> 16) & 0xffff;
    desc->selector = selector;
}

int gdt_alloc_desc()
{
    mutex_lock(&mutex);
    for (int i = 1; i < GDT_TABLE_SIZE; i++)
    {
        if (gdt_table[i].attr == 0)
        {
            mutex_unlock(&mutex);
            return i * sizeof(segment_desc_t);
        }
    }
    mutex_unlock(&mutex);
    return -1;
}


void gdt_init(){
    for (int i = 0; i < GDT_TABLE_SIZE; i++)
    {
        segment_desc_set(i * sizeof(segment_desc_t), 0, 0, 0);
    }

    segment_desc_set(KERNEL_SELECTOR_CS, 0, 0xffffffff, 
        SEG_P_PRESENT | SEG_DPL0 | SEG_S_NORMAL | SEG_TYPE_CODE
        | SEG_TYPE_RW | SEG_D
    );

    segment_desc_set(KERNEL_SELECTOR_DS, 0, 0xffffffff, 
        SEG_P_PRESENT | SEG_DPL0 | SEG_S_NORMAL | SEG_TYPE_DATA
        | SEG_TYPE_RW | SEG_D
    );

    //调用门
    gate_desc_set((gate_desc_t*)(gdt_table + (SELECTOR_SYSCALL >> 3)),
     KERNEL_SELECTOR_CS, (uint32_t)exception_handler_syscall,
        SEG_P_PRESENT | SEG_DPL3 | GATE_TYPE_SYSCALL | SYSCALL_PARAM_COUNT
    );

    lgdt((uint32_t)gdt_table, sizeof(gdt_table));
}

void cpu_init()
{
    gdt_init();
    mutex_init(&mutex);
}

void switch_tss(uint32_t sel)
{
    far_jump(sel, 0);
}

void gdt_free_sel(int sel)
{
    mutex_lock(&mutex);
    gdt_table[sel / sizeof(segment_desc_t)].attr = 0;
    mutex_unlock(&mutex);
}