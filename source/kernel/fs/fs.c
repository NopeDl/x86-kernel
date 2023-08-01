#include "fs/fs.h"
#include "comm/types.h"
#include "tools/klib.h"
#include "comm/cpu_instr.h"
#include "comm/boot_info.h"

static uint8_t TEMP_ADDR[100 * 1024];
static uint8_t* temp_pos;
#define TEMP_FILE_ID 100

/**
 * 这是简易版文件系统，直接读写固定位置处elf
 */
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
    while (sector_count--) {
        while ((inb(0x1f7) & 0x88) != 0x8) {
            /* code */
        }

        // 读取
        for (int i = 0; i < SECTOR_SIZE / 2; i++) {
            *data_buf++ = inw(0x1f0);
        }
    }
}

int sys_fopen(const char* filename, int flags, ...)
{
    // 临时
    if (filename[0] == '/') {
        read_disk(5000, 80, (uint8_t*)TEMP_ADDR);
        temp_pos = (uint8_t*)TEMP_ADDR;
        return TEMP_FILE_ID;
    }
    return -1;
}

int sys_fread(int file, char* ptr, int len)
{
    // 临时
    if (file == TEMP_FILE_ID) {
        kernel_memcpy(ptr, temp_pos, len);
        temp_pos += len;
        return len;
    }

    return -1;
}

int sys_fwrite(int file, char* ptr, int len)
{
    return -1;
}

int sys_lseek(int file, int ptr, int dir)
{
    if (file == TEMP_FILE_ID) {
        temp_pos = (uint8_t*)(TEMP_ADDR + ptr);
        return 0;
    }

    return -1;
}

int sys_fclose(int file)
{
    return 0;
}