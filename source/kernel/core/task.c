#include "core/task.h"
#include "tools/klib.h"

static int tss_init(task_t *task, uint32_t entry, uint32_t esp)
{
    return 0;
}

int task_init(task_t *task, uint32_t entry, uint32_t esp)
{
    ASSERT(task != (task_t *)0);
    tss_init(task, entry, esp);
    return 0;
}

