#ifndef SYSCALL_H
#define SYSCALL_H

#define SYSCALL_PARAM_COUNT 5

typedef struct
{
    int eflags;
    int gs, fs, es, ds;
    int edi, esi, ebp, dummy, ebx, edx, ecx, eax;
    int eip, cs;
    int func_id, arg0, arg1, arg2, arg3;
    int esp, ss;
} syscall_frame_t;

#define SYS_SLEEP 0
#define GET_PID 1
#define SIMPLE_PRINTF 2
#define SYS_FORK 3

void exception_handler_syscall();
void do_handler_syscall(syscall_frame_t* frame);

#endif