#ifndef CPU_INSTR_H
#define CPU_INSTR_H

#include "types.h"

static inline void cli(void)
{
    __asm__ __volatile__("cli");
}

static inline void sti(void)
{
    __asm__ __volatile__("sti");
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ __volatile__("inb %[p], %[v]"
                         : [v] "=a"(ret)
                         : [p] "d"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t data)
{
    __asm__ __volatile__("outb %[v], %[p]" ::[v] "a"(data), [p] "d"(port));
}

static inline void lgdt(uint32_t begin, uint32_t size)
{
    struct
    {
        uint16_t limit;
        uint32_t start15_0;
        uint32_t start31_16;
    } gdt;
    gdt.start31_16 = begin >> 16;
    gdt.start15_0 = begin & 0xffff;
    gdt.limit = size - 1;

    __asm__ __volatile__("lgdt %[g]" ::[g] "m"(gdt));
}

static inline uint16_t read_cr0(void)
{
    uint32_t ret;
    __asm__ __volatile__("mov %%cr0, %[v]":[v]"=a"(ret));
    return ret;
}

static inline void write_cr0(uint32_t data)
{
    __asm__ __volatile__("mov %[v], %%cr0"::[v]"a"(data));
}

#endif