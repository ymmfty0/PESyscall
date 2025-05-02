# Hell's Gate
# Dynamic system call invocation
#
# by smelly__vx (@RtlMateusz) and am0nsec (@am0nsec)

.data
    .lcomm wSystemCall, 4

.text
    .globl HellsGate
HellsGate:
    movl $0, wSystemCall(%rip)
    movl %ecx, wSystemCall(%rip)
    ret

    .globl HellDescent
    .globl HellDescent_stub
HellDescent_stub:
HellDescent:
    movq %rcx, %r10
    movl wSystemCall(%rip), %eax
    syscall
    ret