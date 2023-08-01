#ifndef LIB_SYSCALL_H
#define LIB_SYSCALL_H

#include "comm/types.h"
#include "core/syscall.h"
#include "os_cfg.h"

typedef struct _syscall_args_t {
    // 函数id
    int id;
    // 参数
    int arg0;
    int arg1;
    int arg2;
    int arg3;
} syscall_args_t;

static inline int sys_call(syscall_args_t* args)
{
    uint32_t addr[] = { 0, SELECTOR_SYSCALL | 0 };
    int ret;

    __asm__ __volatile__(
        "push %[arg3]\n\t"
        "push %[arg2]\n\t"
        "push %[arg1]\n\t"
        "push %[arg0]\n\t"
        "push %[id]\n\t"
        "lcalll *(%[a])"
        : "=a"(ret)
        :
        [arg3] "r"(args->arg3),
        [arg2] "r"(args->arg2),
        [arg1] "r"(args->arg1),
        [arg0] "r"(args->arg0),
        [id] "r"(args->id),
        [a] "r"(addr));

    return ret;
}

static inline void msleep(int ms)
{
    if (ms <= 0) {
        return;
    }
    syscall_args_t args;
    args.id = SYS_SLEEP;
    args.arg0 = ms;
    sys_call(&args);
    return;
}

static inline int get_pid()
{
    syscall_args_t args;
    args.id = GET_PID;
    return sys_call(&args);
}

static inline int fork()
{
    syscall_args_t args;
    args.id = SYS_FORK;
    return sys_call(&args);
}

/**
 * 被用于在一个正在运行的进程内部加载另一个可执行程序，并替换当前进程的执行映像
 * @param filename 文件路径
 * @param argv 参数
 * @param envp 环境变量
 */
static inline int execve(const char* filename, char* const argv[], char* const envp[])
{
    syscall_args_t args;
    args.id = SYS_EXECVE;
    args.arg0 = (int)filename;
    args.arg1 = (int)argv;
    args.arg2 = (int)envp;
    return sys_call(&args);
}

static inline int yield()
{
    syscall_args_t args;
    args.id = SYS_YIELD;
    return sys_call(&args);
}

static inline void simple_printf(const char* msg, int arg1)
{
    syscall_args_t args;
    args.id = SIMPLE_PRINTF;
    args.arg0 = (int)msg;
    args.arg1 = arg1;
    sys_call(&args);
    return;
}

#endif