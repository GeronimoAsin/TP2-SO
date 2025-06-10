section .text
global beep

beep:
    push rbp
    mov rbp, rsp


    push rax
    push rcx
    push rdx

    ; Configuro el PIT canal 2 (speaker)
    mov al, 0xB6        ; Canal 2, modo 3 (onda cuadrada)
    out 0x43, al

    ; Frecuencia - 1193180 / 440 ≈ 2712
    mov dx, 2712
    mov al, dl
    out 0x42, al
    mov al, dh
    out 0x42, al

    ; Activo el speaker
    in al, 0x61
    or al, 3
    out 0x61, al

    ; Loop para que se escuche el sonido
    mov rcx, 10000000
.loop:
    nop
    nop                 ; nop = "no hacer nada"
    dec rcx
    jnz .loop

    ; Apago el speaker
    in al, 0x61
    and al, 0xFC
    out 0x61, al


    pop rdx
    pop rcx
    pop rax
    pop rbp
    ret