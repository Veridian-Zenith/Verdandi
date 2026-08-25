/* Bootstrap: Multiboot1 drops us in 32-bit protected mode. We page-table
   ourselves into long mode and hand off to kmain compiled as x86_64.
   Selector layout here (null, code64@0x08, data@0x10) must match
   arch::Gdt::set_flat_lm() so the runtime GDT swaps in without a far jump. */
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

.align 4096
pml4:
.skip 4096
pdpt:
.skip 4096
pd:
.skip 16384                     /* 4 directories x 512 entries = 4 GiB */

mb_info_save:
.skip 8

.section .rodata
.align 16
boot_gdt:
    .quad 0
    .quad 0x00AF9A000000FFFF     /* code64: P ring0 L=1 G=1 */
    .quad 0x00CF92000000FFFF     /* data: P ring0 W G */
boot_gdt_desc:
    .word boot_gdt_desc - boot_gdt - 1
    .long boot_gdt               /* 32-bit lgdt reads only these 6 bytes */

.section .text
.code32
.global _start
.type _start, @function
.extern kmain
_start:
    mov %ebx, mb_info_save       /* multiboot info physical address */
    cli
    mov $stack_top, %esp

    /* identity-map the full 4 GiB with 2 MiB pages (covers kernel + MMIO) */
    mov $pdpt, %eax
    or  $0x03, %eax              /* present | writable */
    mov %eax, pml4

    mov $pdpt, %edi              /* pdpt[0..3] -> the four directories */
    mov $pd, %eax
    or  $0x03, %eax
    mov $4, %ecx
1:  mov %eax, (%edi)
    add $0x1000, %eax
    add $8, %edi
    loop 1b

    mov $pd, %edi                /* every 2 MiB page of the first 4 GiB */
    mov $0x83, %eax              /* P | W | PS */
    mov $2048, %ecx
2:  mov %eax, (%edi)
    add $0x200000, %eax
    add $8, %edi
    loop 2b
    mov $pd, %edi
    mov $0x83, %eax              /* first 2 MiB page: P | W | PS */
    mov $512, %ecx
1:  mov %eax, (%edi)
    add $0x200000, %eax
    add $8, %edi
    loop 1b

    lgdt boot_gdt_desc
    mov %cr4, %eax
    or  $0x20, %eax              /* PAE */
    mov %eax, %cr4
    mov $pml4, %eax
    mov %eax, %cr3
    mov $0xC0000080, %ecx        /* IA32_EFER */
    rdmsr
    or  $0x100, %eax             /* LME */
    wrmsr
    mov %cr0, %eax
    or  $0x80010000, %eax        /* PG (PE already set by bootloader) */
    mov %eax, %cr0
    ljmp $0x08, $long_mode_entry

.code64
long_mode_entry:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    mov $stack_top, %rsp
    xor %ebp, %ebp
    xor %edi, %edi
    mov mb_info_save(%rip), %edi
    call kmain
.hang:
    cli
    hlt
    jmp .hang
.size _start, . - _start
