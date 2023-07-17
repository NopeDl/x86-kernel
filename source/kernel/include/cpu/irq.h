#ifndef IRQ_H
#define IRQ_H
#include "comm/types.h"

#define IRQ0_DE     0
#define IRQ1_DB     1
#define IRQ2_NMI     2
#define IRQ3_BP     3
#define IRQ4_OF     4
#define IRQ5_BR     5
#define IRQ6_UD     6
#define IRQ7_NM     7
#define IRQ8_DF     8
#define IRQ9_DE     9
#define IRQ10_TS     10
#define IRQ11_NP     11
#define IRQ12_SS     12
#define IRQ13_GP     13
#define IRQ14_PF     14

#define IRQ16_MF     16
#define IRQ17_AC     17
#define IRQ18_MC     18
#define IRQ19_XM     19
#define IRQ20_VE     20
#define IRQ21_CP     21

#define PIC0_ICW1        0X20
#define PIC0_ICW2        0X21
#define PIC0_ICW3        0X21
#define PIC0_ICW4        0X21
#define PIC0_IMR         0X21

#define PIC1_ICW1        0XA0
#define PIC1_ICW2        0XA1
#define PIC1_ICW3        0XA1
#define PIC1_ICW4        0XA1
#define PIC1_IMR         0XA1

#define PIC_ICW1_ALWAYS_1       (1 << 4)
#define PIC_ICW1_ICW4           (1 << 0)
#define PIC_ICW4_8086           (1 << 0)
#define IRQ_PIC_START            0X20



typedef struct 
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t num, code;
    uint32_t eip, cs, eflags;
}excption_frame_t;

typedef void (*exception_handle_t)(void);

void irq_init();
void irq_install(int num, exception_handle_t handle);

void exception_handle_unknown(void);
void exception_handle_divider(void);
void exception_handle_debug(void);
void exception_handle_nmi(void);
void exception_handle_breakpoint(void);
void exception_handle_overflow(void);
void exception_handle_bound_range_exceeded(void);
void exception_handle_invalid_opcode(void);
void exception_handle_device_unavalible(void);
void exception_handle_double_fault(void);
void exception_handle_invalid_tss(void);
void exception_handle_seg_not_present(void);
void exception_handle_stack_seg_fault(void);
void exception_handle_general_protection(void);
void exception_handle_page_fault(void);
void exception_handle_math_fault(void);
void exception_handle_alignment_check(void);
void exception_handle_machine_check(void);
void exception_handle_simd_floating_point_exception(void);
void exception_handle_virtualization_exception(void);
void exception_handle_control_protection_exception(void);

void irq_enable_global();
void irq_disable_global();
void irq_enable(int irq_num);
void irq_disable(int irq_num);


#endif