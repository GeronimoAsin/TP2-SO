global schedules
global fill_stack

global idle
extern schedule

section .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

schedules:
    int 0x20
    ret

; fill_stack(rdi = stack_top, rsi = entry_point, rdx = argc, rcx = argv)
; Prepara el stack de un nuevo proceso para ser ejecutado por el scheduler
fill_stack:
    push rbp
    mov rbp, rsp

    mov r10, rdi        ; r10 = stack_top
    mov r11, rsi        ; r11 = entry_point
    mov r12, rdx        ; r12 = argc
    mov r13, rcx        ; r13 = argv

    ; El stack crece hacia abajo, así que empezamos desde el tope
    and r10, 0xFFFFFFFFFFFFFFF0

    mov rsp, r10

    ; Armado del Stack para IRETQ
    push 0x0            ; SS
    push r10            ; RSP (será actualizado después)
    push 0x202          ; RFLAGS (interrupciones habilitadas, IF=1)
    push 0x8            ; CS (segmento de código kernel)
    push r11            ; RIP (entry point del proceso)

    ; Guardado de los registros para popState
    push 0              ; rax
    push 0              ; rbx
    push 0              ; rcx
    push 0              ; rdx
    push r10            ; rbp = stack_top inicial
    push r12            ; rdi = argc (popState lo restaurará)
    push r13            ; rsi = argv (popState lo restaurará)
    push 0              ; r8
    push 0              ; r9
    push 0              ; r10
    push 0              ; r11
    push 0              ; r12
    push 0              ; r13
    push 0              ; r14
    push 0              ; r15


    mov qword [rsp + 15*8 + 3*8], r10  ; Actualizar RSP en frame de iretq

    mov rax, rsp

    mov rsp, rbp
    pop rbp
    ret


idle:
    hlt
    jmp idle