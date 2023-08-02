#include "lib_syscall.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    printf("hello from shell\n");
    while (1) {
        msleep(1000);
    }

    return 0;
}
