/* Interrupt entry points (long mode). Stack layout produced here must match
   arch::Registers exactly; see idt.hpp. */
.code64

.macro ISR_NOERR vect
isr\vect:
    push $0
    push $\vect
    jmp isr_common
.endm

.macro ISR_ERR vect
isr\vect:
    push $\vect            /* CPU already pushed the error code */
    jmp isr_common
.endm

.section .text

isr_common:
    push %rax
    push %rcx
    push %rdx
    push %rbx
    lea 32(%rsp), %rax     /* caller rsp before these four pushes */
    push %rax              /* orig_rsp slot */
    push %rbp
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15
    cld
    mov %rsp, %rdi
    call idt_common        /* arg passed in rdi; nothing to pop */
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rbp
    add $8, %rsp           /* skip orig_rsp slot */
    pop %rbx
    pop %rdx
    pop %rcx
    pop %rax
    add $16, %rsp          /* vector + error */
    iretq

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31
ISR_NOERR 32
ISR_NOERR 33
ISR_NOERR 34
ISR_NOERR 35
ISR_NOERR 36
ISR_NOERR 37
ISR_NOERR 38
ISR_NOERR 39
ISR_NOERR 40
ISR_NOERR 41
ISR_NOERR 42
ISR_NOERR 43
ISR_NOERR 44
ISR_NOERR 45
ISR_NOERR 46
ISR_NOERR 47

.section .rodata
.global isr_stub_table
.align 8
isr_stub_table:
    .quad isr0
    .quad isr1
    .quad isr2
    .quad isr3
    .quad isr4
    .quad isr5
    .quad isr6
    .quad isr7
    .quad isr8
    .quad isr9
    .quad isr10
    .quad isr11
    .quad isr12
    .quad isr13
    .quad isr14
    .quad isr15
    .quad isr16
    .quad isr17
    .quad isr18
    .quad isr19
    .quad isr20
    .quad isr21
    .quad isr22
    .quad isr23
    .quad isr24
    .quad isr25
    .quad isr26
    .quad isr27
    .quad isr28
    .quad isr29
    .quad isr30
    .quad isr31
    .quad isr32
    .quad isr33
    .quad isr34
    .quad isr35
    .quad isr36
    .quad isr37
    .quad isr38
    .quad isr39
    .quad isr40
    .quad isr41
    .quad isr42
    .quad isr43
    .quad isr44
    .quad isr45
    .quad isr46
    .quad isr47

/* Divide-by-zero recovery gate test: fault lands on the div, handler
   resumes at gate_div_resume_ip. */
.section .text
.global gate_div_run
.global gate_div_fault_ip
.global gate_div_resume_ip
.type gate_div_run, @function
gate_div_run:
    xor %ebx, %ebx
gate_div_fault_ip:
    div %ebx
gate_div_resume_ip:
    ret
.size gate_div_run, . - gate_div_run
