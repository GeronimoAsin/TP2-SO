global getTimeAsm

getTimeAsm:
    push rbp
    mov rbp, rsp

    call getSeconds
    mov rbx, rax          ; rbx = seconds

    call getMinutes
    mov rcx, rax
    mov rax, rcx
    mov rdx, 60
    mul rdx               ; rax = minutes * 60
    add rbx, rax          ; rbx += minutes in seconds

    call getHours
    mov rcx, rax
    mov rax, rcx
    mov rdx, 3600
    mul rdx               ; rax = hours * 3600
    add rbx, rax          ; rbx += hours in seconds

    mov rax, rbx          ; Return total seconds in rax

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