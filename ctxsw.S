.section .text
.global ctxsw

ctxsw:
    pushal
    movl 36(%esp), %eax
    movl %esp, (%eax)
    movl 40(%esp), %esp
    popal
    ret
