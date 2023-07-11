#include "loader.h"

static void read_disk(uint32_t sector, uint32_t sector_count, uint8_t* buffer)
{
    outb(0x1f6, 0xe0);
    outb(0x1f2, (uint8_t)(sector_count >> 8));
    outb(0x1f3, (uint8_t)(sector >> 24));
    outb(0x1f4, 0);
    outb(0x1f5, 0);

    outb(0x1f2, (uint8_t)sector_count);
    outb(0x1f3, (uint8_t)sector);
    outb(0x1f4, (uint8_t)(sector >> 8));
    outb(0x1f5, (uint8_t)(sector >> 16));

    outb(0x1f7, 0x24);

    uint16_t* data_buf = (uint16_t*)buffer;
    while (sector_count--)
    {
        while ((inb(0x1f7) & 0x88) != 0x8)
        {
            /* code */
        }

        //读取
        for(int i = 0; i < SECTOR_SIZE / 2; i++){
            *data_buf++ = inw(0x1f0);
        }
        
    }
    
}

void load_kernel(void)
{
    read_disk(100, 500, (uint8_t*)SYS_KERNEL_LOAD_ADDR);
    ((void (*)(boot_info_t*))SYS_KERNEL_LOAD_ADDR)(&boot_info);
}