section .text
global beep

beep:
    push rbp
    mov rbp, rsp

    ; Guardar registros que vamos a usar
    push rax
    push rcx
    push rdx

    ; Configurar el PIT canal 2 (speaker)
    mov al, 0xB6        ; Canal 2, modo 3 (onda cuadrada), acceso palabra completa
    out 0x43, al

    ; Frecuencia - 1193180 / 440 ≈ 2712
    mov dx, 2712        ; Usar frecuencia en DX para mayor claridad
    mov al, dl          ; Byte bajo
    out 0x42, al
    mov al, dh          ; Byte alto
    out 0x42, al

    ; Activar el speaker
    in al, 0x61
    or al, 3            ; Activar bits 0 y 1 (gate y data)
    out 0x61, al

    ; Retardo más largo y diferente método
    mov rcx, 10000000   ; Aumentar significativamente el retardo
.loop:
    nop
    nop                 ; Múltiples NOPs para consumir ciclos
    dec rcx
    jnz .loop

    ; Apagar el speaker
    in al, 0x61
    and al, 0xFC        ; Desactivar explícitamente bits 0-1
    out 0x61, al

    ; Restaurar registros
    pop rdx
    pop rcx
    pop rax
    pop rbp
    ret