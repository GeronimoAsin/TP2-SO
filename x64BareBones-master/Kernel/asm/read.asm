global read

section .text

read:
    ; Protocolo estándar de función
    push rbp
    mov rbp, rsp

    ; Leer del puerto 0x60 → AL
    mov dx, 0x60
    in al, dx

    ; Guardar AL en [RDI] (primer argumento es puntero destino)
    mov [rdi], al

    ; Restaurar y salir
    pop rbp
    ret