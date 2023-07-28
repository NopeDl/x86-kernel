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

static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    __asm__ __volatile__("in %[p], %[v]"
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
        uint16_t start15_0;
        uint16_t start31_16;
    } gdt;
    gdt.start31_16 = begin >> 16;
    gdt.start15_0 = begin & 0xffff;
    gdt.limit = size - 1;

    __asm__ __volatile__("lgdt %[g]" ::[g] "m"(gdt));
}

static inline void lidt(uint32_t begin, uint32_t size)
{
    struct
    {
        uint16_t limit;
        uint16_t start15_0;
        uint16_t start31_16;
    } idt;
    idt.start31_16 = begin >> 16;
    idt.start15_0 = begin & 0xffff;
    idt.limit = size - 1;

    __asm__ __volatile__("lidt %[g]" ::[g] "m"(idt));
}

static inline uint32_t read_cr0(void)
{
    uint32_t ret;
    __asm__ __volatile__("mov %%cr0, %[v]"
                         : [v] "=a"(ret));
    return ret;
}

static inline void write_cr0(uint32_t data)
{
    __asm__ __volatile__("mov %[v], %%cr0" ::[v] "a"(data));
}

static inline uint32_t read_cr2(void)
{
    uint32_t ret;
    __asm__ __volatile__("mov %%cr2, %[v]"
                         : [v] "=a"(ret));
    return ret;
}


static inline uint32_t read_cr3(void)
{
    uint32_t ret;
    __asm__ __volatile__("mov %%cr3, %[v]"
                         : [v] "=a"(ret));
    return ret;
}

static inline void write_cr3(uint32_t data)
{
    __asm__ __volatile__("mov %[v], %%cr3" ::[v] "a"(data));
}

static inline uint32_t read_cr4(void)
{
    uint32_t ret;
    __asm__ __volatile__("mov %%cr4, %[v]"
                         : [v] "=a"(ret));
    return ret;
}

static inline void write_cr4(uint32_t data)
{
    __asm__ __volatile__("mov %[v], %%cr4" ::[v] "a"(data));
}





static inline void far_jump(uint32_t selector, uint32_t offset)
{
    uint32_t addr[] = {offset, selector};
    __asm__ __volatile__("ljmpl *(%[a])" ::[a] "r"(addr));
}



static inline void hlt()
{
    __asm__ __volatile__("hlt");
}

static inline void write_tr(uint16_t tss_sel)
{
    __asm__ __volatile__("ltr %%ax" ::"a"(tss_sel));
}

static inline uint32_t read_eflags()
{
    uint32_t ret = 0;
    __asm__ __volatile__("pushfl\n\tpopl %%eax"
                         : "=a"(ret));
    return ret;
}

static inline void write_eflags(uint32_t state)
{
    __asm__ __volatile__("pushl %%eax\n\tpopfl" ::"a"(state));
}

#endif