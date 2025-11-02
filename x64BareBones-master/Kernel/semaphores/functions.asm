global acquire
global release
section .text

acquire:
    mov al, 1
    xchg al, [spinlock]
    cmp al, 0
    jne acquire
    ret

release:
    mov byte [spinlock], 0
    ret

section .bss
spinlock resb 1
