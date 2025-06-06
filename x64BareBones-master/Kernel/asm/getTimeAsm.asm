global getTimeAsm

global getSeconds
global getMinutes
global getHours

getTimeAsm:
    push rbp
    mov rbp, rsp

    call getSeconds
    mov rcx, rax          ; rcx = seconds

    call getMinutes
    mov rbx, rax          ; rbx = minutes

    call getHours
    mov rax, rax          ; rax = hours

    ; rax = hours, rbx = minutes, rcx = seconds
    mov rsp, rbp
    pop rbp
    ret

getSeconds:
    push rbp
    mov rbp, rsp

    mov al, 0x00
    out 0x70, al
    in al, 0x71
    movzx rax, al

    pop rbp
    ret

getMinutes:
    push rbp
    mov rbp, rsp

    mov al, 0x02
    out 0x70, al
    in al, 0x71
    movzx rax, al

    pop rbp
    ret

getHours:
    push rbp
    mov rbp, rsp

    mov al, 0x04
    out 0x70, al
    in al, 0x71
    movzx rax, al
    pop rbp
    ret