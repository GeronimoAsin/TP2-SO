GLOBAL syscall
GLOBAL invalidOp
section .text

;puente para invocar las syscalls del kernel en Userland
syscall:
    push rbx
    mov rax, rdi
    mov rbx, rsi
    int 80h
    pop rbx
    ret

invalidOp:
    mov ax, cs