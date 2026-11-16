.section .text
.global ctxsw

ctxsw:
    pushfl
    pushal
    movl 40(%esp), %eax
    movl %esp, (%eax)
    movl 44(%esp), %esp
    popal
    popfl
    ret
