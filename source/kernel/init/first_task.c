#include "applib/lib_syscall.h"

int first_task_main()
{
    int count = 3;

    int pid = get_pid();
    simple_printf("first task id = %d\n", pid);

    pid = fork();
    if (pid < 0) {
        simple_printf("create child proc failed\n", 0);
    } else if (pid == 0) {
        simple_printf("child: %d\n", count);

        char* argv[] = { "arg0", "arg1", "arg2" };
        execve("/shell.elf", argv, (char**)0);
    } else {
        simple_printf("child task id = %d\n", pid);
        simple_printf("parent: %d\n", count);
    }

    while (1) {
        simple_printf("task id: %d", pid);
        msleep(1000);
    }

    return 0;
}