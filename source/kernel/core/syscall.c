#include "core/syscall.h"
#include "core/task.h"
#include "comm/types.h"
#include "tools/log.h"

typedef int (*syscall_handle_t)(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3);

static const syscall_handle_t sys_func_table[] = {
    [SYS_SLEEP] = (syscall_handle_t)sys_sleep,
};

void do_handler_syscall(syscall_frame_t *frame)
{
    if (frame->func_id >= 0 && frame->func_id < sizeof(sys_func_table) / sizeof(syscall_handle_t))
    {
        syscall_handle_t handler = sys_func_table[frame->func_id];
        if (handler)
        {
            int ret = handler(frame->arg0, frame->arg1, frame->arg2, frame->arg3);
            return;
        }
    }
    log_printf("error syscall task: %s, func_id: %d", get_task_cur()->name, frame->func_id);
    return;
}