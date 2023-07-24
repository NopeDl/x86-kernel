#ifndef BITMAP_H
#define BITMAP_H

#include "comm/types.h"

typedef struct 
{
    int bit_count;
    uint8_t *bits;
}bitmap_t;

void bitmap_init(bitmap_t* bitmap, uint8_t *bits, int count, int init_bit);
int bitmap_byte_count(int bit_count);

#endif