.section .text
.global timer_interrupt_handler
.extern timer_interrupt_2

timer_interrupt_handler:
    pushal
    cld
    call timer_interrupt_2
    popal
    iret