#include "os_cfg.h"

    .text
    .extern kernel_init
    .extern init_main
    .global _start
_start:
    push %ebp
    mov %esp, %ebp
    mov 0x8(%ebp), %eax

    push %eax
    call kernel_init
    jmp $KERNEL_SELECTOR_CS, $gdt_reload

gdt_reload:
    mov $KERNEL_SELECTOR_DS, %ax
	mov %ax, %ss
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

    mov $(stack + KERNEL_STACK_SIZE), %esp
    jmp init_main

    .bss
.comm stack, KERNEL_STACK_SIZE


.macro exception_handle name num code
    .text
    .extern do_handle_\name
    .global exception_handle_\name
exception_handle_\name:
    .if \code == 0
        push $0
    .endif
    push $\num
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    push %esp
    call do_handle_\name
    add $(1*4), %esp
    
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    add $(2*4), %esp
    
    iret
.endm

exception_handle unknown -1 0
exception_handle divider 0 0
exception_handle debug 1 0
exception_handle nmi 2 0
exception_handle breakpoint 3 0
exception_handle overflow 4 0
exception_handle bound_range_exceeded 5 0
exception_handle invalid_opcode 6 0
exception_handle device_unavalible 7 0
exception_handle double_fault 8 1

exception_handle invalid_tss 10 1
exception_handle seg_not_present 11 1
exception_handle stack_seg_fault 12 1
exception_handle general_protection 13 1
exception_handle page_fault 14 1

exception_handle math_fault 16 0
exception_handle alignment_check 17 1
exception_handle machine_check 18 0
exception_handle simd_floating_point_exception 19 0
exception_handle virtualization_exception 20 0
exception_handle control_protection_exception 21 1

exception_handle timer 0x20 0