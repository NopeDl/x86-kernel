/**
 * 这行指令用于让编译器生成16位代码
 */
__asm__(".code16gcc");

void show_msg(char* msg)
{
    char c;
    while((c = *msg++) != '\0'){
        __asm__ __volatile__(
            "mov $0xe, %%ah\n\t"
            "mov %[ch], %%al\n\t"
            "int $0x10"::[ch]"r"(c)
        );
    }
}


void loader_entry(void)
{
    show_msg("hello,world");
    for (;;)
    {
    }
}