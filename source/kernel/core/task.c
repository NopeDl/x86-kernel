#include "core/task.h"
#include "tools/klib.h"
#include "os_cfg.h"
#include "cpu/cpu.h"

static int tss_init(task_t *task, uint32_t entry, uint32_t esp)
{
    tss_t *tp = &task->tss;
    kernel_memset(tp, 0, sizeof(tss_t));
    tp->esp = tp->esp0 = esp;
    tp->eip = entry;
    tp->ss = tp->ss0 = KERNEL_SELECTOR_DS;
    tp->es = tp->ds = tp->fs = tp->gs = KERNEL_SELECTOR_DS;
    tp->cs = KERNEL_SELECTOR_CS;
    tp->eflags = EFLAGS_DEFAULT | EFLAGS_IF;
    return 0;
}

int task_init(task_t *task, uint32_t entry, uint32_t esp)
{
    ASSERT(task != (task_t *)0);
    tss_init(task, entry, esp);
    return 0;
}
