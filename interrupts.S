.section .text
.intel_syntax noprefix
.global irq_save
.global irq_restore

irq_save:
    pushfd
    pop eax
    cli
    ret

irq_restore:
    push dword [esp + 4]
    popfd
    ret