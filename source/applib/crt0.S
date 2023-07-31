    .text
    .extern cstart
    .global _start
_start:
    mov %ss, %ax
	mov %ax, %ss
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	jmp cstart
    