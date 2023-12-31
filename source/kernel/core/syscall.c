#include "core/syscall.h"
#include "comm/types.h"
#include "core/task.h"
#include "tools/log.h"
#include "fs/fs.h"
#include "core/memory.h"

typedef int (*syscall_handle_t)(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3);

static const syscall_handle_t sys_func_table[] = {
    [SYS_SLEEP] = (syscall_handle_t)sys_sleep,
    [GET_PID] = (syscall_handle_t)sys_get_pid,
    [SYS_FORK] = (syscall_handle_t)sys_fork,
    [SYS_EXECVE] = (syscall_handle_t)sys_execve,
    [SYS_YIELD] = (syscall_handle_t)sys_sched_yield,

    [SYS_OPEN] = (syscall_handle_t)sys_fopen,
    [SYS_READ] = (syscall_handle_t)sys_fread,
    [SYS_WRITE] = (syscall_handle_t)sys_fwrite,
    [SYS_CLOSE] = (syscall_handle_t)sys_fclose,
    [SYS_LSEEK] = (syscall_handle_t)sys_lseek,

    [SYS_ISATTY] = (syscall_handle_t)sys_isatty,
    [SYS_FSTAT] = (syscall_handle_t)sys_fstat,
    [SYS_SBRK] = (syscall_handle_t)sys_sbrk,
    
    [SIMPLE_PRINTF] = (syscall_handle_t)log_printf,
};

void do_handler_syscall(syscall_frame_t* frame)
{
    if (frame->func_id >= 0 && frame->func_id < sizeof(sys_func_table) / sizeof(syscall_handle_t)) {
        syscall_handle_t handler = sys_func_table[frame->func_id];
        if (handler) {
            int ret = handler(frame->arg0, frame->arg1, frame->arg2, frame->arg3);
            frame->eax = ret;
            return;
        }
    }
    log_printf("error syscall task: %s, func_id: %d", get_task_cur()->name, frame->func_id);
    frame->eax = -1;
    return;
}