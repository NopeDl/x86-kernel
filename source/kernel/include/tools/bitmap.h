#ifndef BITMAP_H
#define BITMAP_H

#include "comm/types.h"

typedef struct
{
    int bit_count;
    uint8_t *bits;
} bitmap_t;

void bitmap_init(bitmap_t *bitmap, uint8_t *bits, int count, int init_bit);
int bitmap_byte_count(int bit_count);

int bitmap_get_bit(bitmap_t *bitmap, int index);
void bitmap_set_bit(bitmap_t *bitmap, int index, int count, int bit);
int bitmap_is_set(bitmap_t *bitmap, int index);
int bitmap_alloc_nbits(bitmap_t *bitmap, int bit, int count);
#endif