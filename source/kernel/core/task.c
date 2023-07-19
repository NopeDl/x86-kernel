#include "core/task.h"
#include "tools/klib.h"
#include "os_cfg.h"
#include "tools/log.h"
#include "cpu/cpu.h"

static int tss_init(task_t *task, uint32_t entry, uint32_t esp)
{
    int tss_selector = gdt_alloc_desc();
    if (tss_selector == -1)
    {
        log_printf("alloc tss failed....");
        return -1;
    }
    tss_t *tp = &task->tss;
    segment_desc_set(tss_selector, (uint32_t)tp, sizeof(tss_t),
                     SEG_P_PRESENT | SEG_DPL0 | SEG_TYPE_TSS);

    kernel_memset(tp, 0, sizeof(tss_t));
    tp->esp = tp->esp0 = esp;
    tp->eip = entry;
    tp->ss = tp->ss0 = KERNEL_SELECTOR_DS;
    tp->es = tp->ds = tp->fs = tp->gs = KERNEL_SELECTOR_DS;
    tp->cs = KERNEL_SELECTOR_CS;
    tp->eflags = EFLAGS_DEFAULT | EFLAGS_IF;
    task->tss_sel = tss_selector;
    return 0;
}

int task_init(task_t *task, uint32_t entry, uint32_t esp)
{
    ASSERT(task != (task_t *)0);
    // tss_init(task, entry, esp);

    uint32_t * pesp = (uint32_t *)esp;
    if (pesp)
    {
        *(--pesp) = entry;
        *(--pesp) = 0;
        *(--pesp) = 0;
        *(--pesp) = 0;
        *(--pesp) = 0;
        task->stack = pesp;
    }
    
    return 0;
}

void simple_switch(uint32_t **from, uint32_t *to);

void task_switch_from_to(task_t *from, task_t *to)
{
    // switch_tss(to->tss_sel);
    simple_switch(&from->stack, to->stack);
}