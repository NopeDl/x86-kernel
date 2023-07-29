#include "applib/lib_syscall.h"

int first_task_main()
{
    while (1)
    {
        int ret = get_pid();
        msleep(1000);
        simple_printf("pid: %d", ret);
    }
}