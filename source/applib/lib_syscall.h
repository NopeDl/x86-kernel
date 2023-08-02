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

int sys_call(syscall_args_t* args);

void msleep(int ms);

int get_pid();

int fork();

/**
 * 被用于在一个正在运行的进程内部加载另一个可执行程序，并替换当前进程的执行映像
 * @param filename 文件路径
 * @param argv 参数
 * @param envp 环境变量
 */
int execve(const char* filename, char* const argv[], char* const envp[]);

int yield();

int open(const char* name, int flags, ...);
int read(int file, char* ptr, int len);
int write(int file, char* ptr, int len);
int close(int file);
int lseek(int file, int ptr, int dir);

void simple_printf(const char* msg, int arg1);

#endif