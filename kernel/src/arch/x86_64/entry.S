/* Multiboot1 header must be within the first 8 KiB and 4-byte aligned. */
.set MB_MAGIC, 0x1BADB002
.set MB_FLAGS, 0x00000002

.section .multiboot, "a"
.align 4
.long MB_MAGIC
.long MB_FLAGS
.long -(MB_MAGIC + MB_FLAGS)

.section .bss, "aw", @nobits
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.type _start, @function
.extern kmain
_start:
    mov $stack_top, %esp
    push %ebx                  /* physical address of multiboot info */
    call kmain
.hang:
    cli
    hlt
    jmp .hang
.size _start, . - _start
