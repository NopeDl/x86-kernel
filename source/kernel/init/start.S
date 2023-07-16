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