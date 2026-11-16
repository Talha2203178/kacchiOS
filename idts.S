.section .text
.intel_syntax noprefix
.global set_idt_reg

set_idt_reg:
    mov eax, [esp + 4]
    lidt [eax]
    ret