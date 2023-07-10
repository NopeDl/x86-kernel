#ifndef CPU_INSTR_H
#define CPU_INSTR_H

static inline void cli(void)
{
    __asm__ __volatile__("cli");
}

static inline void sti(void)
{
    __asm__ __volatile__("sti");
}

#endif