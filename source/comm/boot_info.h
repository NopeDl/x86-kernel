#ifndef BOOT_INFO_H
#define BOOT_INFO_H

#include "types.h"

#define BOOT_RAM_REGION_MAX     10
#define SECTOR_SIZE         512
#define SYS_KERNEL_LOAD_ADDR    (1024 * 1024)

typedef struct _boot_info_t
{
    struct
    {
        uint32_t start;
        uint32_t size;
    }ram_region_cfg[BOOT_RAM_REGION_MAX];
    
    int ram_region_count;
    
}boot_info_t;

extern boot_info_t boot_info;

#endif