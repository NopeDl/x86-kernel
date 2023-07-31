#include "cpu/irq.h"
#include "comm/cpu_instr.h"
#include "cpu/cpu.h"
#include "os_cfg.h"
#include "tools/log.h"

#define IDT_TABLE_NR 128

static gate_desc_t idt_table[IDT_TABLE_NR];

static void dump_core_regs(excption_frame_t* frame)
{
    // 打印CPU寄存器相关内容
    uint32_t esp, ss;
    if (frame->cs & 0x7) {
        ss = frame->ds;
        esp = frame->esp;
    } else {
        ss = frame->ss3;
        esp = frame->esp3;
    }
    log_printf("IRQ: %d, error code: %d.", frame->num, frame->code);
    log_printf("CS: %d\nDS: %d\nES: %d\nSS: %d\nFS:%d\nGS:%d",
        frame->cs, frame->ds, frame->es, ss, frame->fs, frame->gs);
    log_printf("EAX:0x%x\n"
               "EBX:0x%x\n"
               "ECX:0x%x\n"
               "EDX:0x%x\n"
               "EDI:0x%x\n"
               "ESI:0x%x\n"
               "EBP:0x%x\n"
               "ESP:0x%x\n",
        frame->eax, frame->ebx, frame->ecx, frame->edx,
        frame->edi, frame->esi, frame->ebp, esp);
    log_printf("EIP:0x%x\nEFLAGS:0x%x\n", frame->eip, frame->eflags);
}

static void do_default_handle(excption_frame_t* frame, const char* msg)
{
    log_printf("------------------------------------------");
    log_printf("IRQ EXCEPTION: %s", msg);
    dump_core_regs(frame);
    log_printf("------------------------------------------");
    hlt();
}

void do_handle_unknown(excption_frame_t* frame)
{
    do_default_handle(frame, "unknown exception");
}

void do_handle_divider(excption_frame_t* frame)
{
    do_default_handle(frame, "divider exception");
}

void do_handle_debug(excption_frame_t* frame)
{
    do_default_handle(frame, "divider exception");
}

void do_handle_nmi(excption_frame_t* frame)
{
    do_default_handle(frame, "nmi exception");
}

void do_handle_breakpoint(excption_frame_t* frame)
{
    do_default_handle(frame, "breakpoint exception");
}

void do_handle_overflow(excption_frame_t* frame)
{
    do_default_handle(frame, "overflow exception");
}

void do_handle_bound_range_exceeded(excption_frame_t* frame)
{
    do_default_handle(frame, "bound_range_exceeded exception");
}

void do_handle_invalid_opcode(excption_frame_t* frame)
{
    do_default_handle(frame, "invalid_opcode exception");
}

void do_handle_device_unavalible(excption_frame_t* frame)
{
    do_default_handle(frame, "device_unavalible exception");
}

void do_handle_double_fault(excption_frame_t* frame)
{
    do_default_handle(frame, "double_fault exception");
}

void do_handle_invalid_tss(excption_frame_t* frame)
{
    do_default_handle(frame, "invalid_tss exception");
}

void do_handle_seg_not_present(excption_frame_t* frame)
{
    do_default_handle(frame, "seg_not_present exception");
}

void do_handle_stack_seg_fault(excption_frame_t* frame)
{
    do_default_handle(frame, "stack_seg_fault exception");
}

void do_handle_general_protection(excption_frame_t* frame)
{
    log_printf("--------------------------------");
    log_printf("IRQ/Exception happend: General Protection.");
    if (frame->code & ERR_EXT) {
        log_printf("the exception occurred during delivery of an "
                   "event external to the program, such as an interrupt"
                   "or an earlier exception.");
    } else {
        log_printf("the exception occurred during delivery of a"
                   "software interrupt (INT n, INT3, or INTO).");
    }

    if (frame->code & ERR_IDT) {
        log_printf("the index portion of the error code refers "
                   "to a gate descriptor in the IDT");
    } else {
        log_printf("the index refers to a descriptor in the GDT");
    }

    log_printf("segment index: %d", frame->code & 0xFFF8);

    dump_core_regs(frame);
    while (1) {
        hlt();
    }
}

void do_handle_page_fault(excption_frame_t* frame)
{
    log_printf("--------------------------------");
    log_printf("IRQ/Exception happend: Page fault.");
    if (frame->code & ERR_PAGE_P) {
        log_printf("\tpage-level protection violation: 0x%x.", read_cr2());
    } else {
        log_printf("\tPage doesn't present 0x%x", read_cr2());
    }

    if (frame->code & ERR_PAGE_WR) {
        log_printf("\tThe access causing the fault was a write.");
    } else {
        log_printf("\tThe access causing the fault was a read.");
    }

    if (frame->code & ERR_PAGE_US) {
        log_printf("\tA user-mode access caused the fault.");
    } else {
        log_printf("\tA supervisor-mode access caused the fault.");
    }

    dump_core_regs(frame);
    while (1) {
        hlt();
    }
}

void do_handle_math_fault(excption_frame_t* frame)
{
    do_default_handle(frame, "math_fault exception");
}

void do_handle_alignment_check(excption_frame_t* frame)
{
    do_default_handle(frame, "alignment_check exception");
}

void do_handle_machine_check(excption_frame_t* frame)
{
    do_default_handle(frame, "machine_check exception");
}

void do_handle_simd_floating_point_exception(excption_frame_t* frame)
{
    do_default_handle(frame, "simd_floating_point_exception exception");
}

void do_handle_virtualization_exception(excption_frame_t* frame)
{
    do_default_handle(frame, "virtualization exception");
}

void do_handle_control_protection_exception(excption_frame_t* frame)
{
    do_default_handle(frame, "control_protection exception");
}

static void init_pic(void)
{
    // 第一块芯片
    outb(PIC0_ICW1, PIC_ICW1_ALWAYS_1 | PIC_ICW1_ICW4);
    // 被异常中断使用了，要跳到另一部分
    outb(PIC0_ICW2, IRQ_PIC_START);
    outb(PIC0_ICW3, 1 << 2);
    outb(PIC0_ICW4, PIC_ICW4_8086);

    // 第二块
    outb(PIC1_ICW1, PIC_ICW1_ALWAYS_1 | PIC_ICW1_ICW4);
    outb(PIC1_ICW2, IRQ_PIC_START + 8);
    outb(PIC1_ICW3, 2);
    outb(PIC1_ICW4, PIC_ICW4_8086);

    // 设置寄存器，先屏蔽中断，暂时不响应，中断处理程序还没好
    // 不禁止第二片8259
    outb(PIC0_IMR, 0XFF & ~(1 << 2));
    // 禁止第二片接受响应
    outb(PIC1_IMR, 0XFF);
}

void irq_init(void)
{
    for (int i = 0; i < IDT_TABLE_NR; i++) {
        gate_desc_set(idt_table + i, KERNEL_SELECTOR_CS, (uint32_t)exception_handle_unknown,
            GATE_P_PRESENT | GATE_DPL0 | GATE_TYPE_INT);
    }

    irq_install(IRQ0_DE, exception_handle_divider);
    irq_install(IRQ1_DB, exception_handle_debug);
    irq_install(IRQ2_NMI, exception_handle_nmi);
    irq_install(IRQ3_BP, exception_handle_breakpoint);
    irq_install(IRQ4_OF, exception_handle_overflow);
    irq_install(IRQ5_BR, exception_handle_bound_range_exceeded);
    irq_install(IRQ6_UD, exception_handle_invalid_opcode);
    irq_install(IRQ7_NM, exception_handle_device_unavalible);
    irq_install(IRQ8_DF, exception_handle_double_fault);
    irq_install(IRQ10_TS, exception_handle_invalid_tss);
    irq_install(IRQ11_NP, exception_handle_seg_not_present);
    irq_install(IRQ12_SS, exception_handle_stack_seg_fault);
    irq_install(IRQ13_GP, exception_handle_general_protection);
    irq_install(IRQ14_PF, exception_handle_page_fault);
    irq_install(IRQ16_MF, exception_handle_math_fault);
    irq_install(IRQ17_AC, exception_handle_alignment_check);
    irq_install(IRQ18_MC, exception_handle_machine_check);
    irq_install(IRQ19_XM, exception_handle_simd_floating_point_exception);
    irq_install(IRQ20_VE, exception_handle_virtualization_exception);
    irq_install(IRQ21_CP, exception_handle_control_protection_exception);

    lidt((uint32_t)idt_table, sizeof(idt_table));

    // 初始化中断控制器
    init_pic();
}

void irq_install(int num, exception_handle_t handle)
{
    if (num >= IDT_TABLE_NR) {
        return;
    }

    gate_desc_set(idt_table + num, KERNEL_SELECTOR_CS, (uint32_t)handle,
        GATE_P_PRESENT | GATE_DPL0 | GATE_TYPE_INT);
}

void irq_enable_global()
{
    sti();
}

void irq_disable_global()
{
    cli();
}

void irq_enable(int irq_num)
{
    if (irq_num < 0) {
        return;
    }

    irq_num -= IRQ_PIC_START;
    if (irq_num < 8) {
        // 第一块8259
        uint8_t imr = inb(PIC0_IMR);
        outb(PIC0_IMR, imr & ~(1 << irq_num));
    } else {
        irq_num -= 8;
        uint8_t imr = inb(PIC1_IMR);
        outb(PIC1_IMR, imr & ~(1 << irq_num));
    }
}

void irq_disable(int irq_num)
{
    if (irq_num < 0) {
        return;
    }

    irq_num -= IRQ_PIC_START;
    if (irq_num < 8) {
        // 第一块8259
        uint8_t imr = inb(PIC0_IMR);
        outb(PIC0_IMR, imr | (1 << irq_num));
    } else {
        irq_num -= 8;
        uint8_t imr = inb(PIC1_IMR);
        outb(PIC1_IMR, imr | (1 << irq_num));
    }
}

void pic_send_eoi(int irq_num)
{
    irq_num -= IRQ_PIC_START;
    if (irq_num >= 8) {
        outb(PIC1_OCW2, PIC_OCW2_EOI);
    }
    outb(PIC0_OCW2, PIC_OCW2_EOI);
}

irq_state irq_enter_protection()
{
    irq_state state = read_eflags();
    irq_disable_global();
    return state;
}

void irq_leave_protection(irq_state state)
{
    write_eflags(state);
}