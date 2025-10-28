extern getStackBase

GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _irq80Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

global getGlobalSavedContext


EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN syscallDispatcher
EXTERN saveRegisters
SECTION .text

%macro pushState 0
	; Guardo RBP en variable temporal antes de que se modifique
	mov [temp_rbp], rbp
	
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

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro



%macro exceptionHandler 1
    pushState

    mov [savedRegisters + 0x00], rax
    mov [savedRegisters + 0x08], rbx
    mov [savedRegisters + 0x10], rcx
    mov [savedRegisters + 0x18], rdx
    mov [savedRegisters + 0x20], rsi
    mov [savedRegisters + 0x28], rdi
    mov [savedRegisters + 0x30], rsp
    mov [savedRegisters + 0x38], rbp
    mov [savedRegisters + 0x40], r8
    mov [savedRegisters + 0x48], r9
    mov [savedRegisters + 0x50], r10
    mov [savedRegisters + 0x58], r11
    mov [savedRegisters + 0x60], r12
    mov [savedRegisters + 0x68], r13
    mov [savedRegisters + 0x70], r14
    mov [savedRegisters + 0x78], r15
    ; RIP
    mov rax, [rsp + 15*8]
    mov [savedRegisters + 0x80], rax
    mov rax, [rsp + 17*8]
    mov [savedRegisters + 0x88], rax
    mov rax, [savedRegisters]
    mov rdi, %1
    mov rsi, [rsp + 15*8] ; RIP
    mov rdx, [rsp + 17*8] ; RFLAGS
    mov rcx, [rsp + 16*8] ; CS
    call exceptionDispatcher

    mov qword [rsp + 15*8], 0x400000

    popState
    iretq
%endmacro

getGlobalSavedContext:
    mov rax, savedRegisters
    ret

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
    mov [savedRegisters + 0x00], rax
    mov [savedRegisters + 0x08], rbx
    mov [savedRegisters + 0x10], rcx
    mov [savedRegisters + 0x18], rdx
    mov [savedRegisters + 0x20], rsi
    mov [savedRegisters + 0x28], rdi
    mov [savedRegisters + 0x30], rsp
    mov [savedRegisters + 0x38], rbp
    mov [savedRegisters + 0x40], r8
    mov [savedRegisters + 0x48], r9
    mov [savedRegisters + 0x50], r10
    mov [savedRegisters + 0x58], r11
    mov [savedRegisters + 0x60], r12
    mov [savedRegisters + 0x68], r13
    mov [savedRegisters + 0x70], r14
    mov [savedRegisters + 0x78], r15
    mov rax, [rsp]  ;RIP
    mov [savedRegisters + 0x80], rax
    mov rax, [rsp + 8] ;CS
    mov [savedRegisters + 0x88], rax
    mov rax, [rsp + 2*8]  ;RFLAGS
    mov [savedRegisters + 0x90], rax
    mov rax, [rsp + 3*8] ;RSP
    mov [savedRegisters + 0x98], rax
    mov rax, [rsp + 4*8] ;SS
    mov [savedRegisters + 0xA0], rax
    mov rax, [rsp +5*8] ;Align
    mov [savedRegisters + 0xA8], rax

	mov rdi, 0
    mov rsi, savedRegisters
    call irqDispatcher

    ; fin de la interrupción
    mov al, 20h
    out 20h, al

    mov rsp, [savedRegisters + 0x30] ;restauro rsp
    mov rax, [savedRegisters + 0xA8] ;restauro align
    mov [rsp + 5*8], rax
    mov rax, [savedRegisters + 0xA0] ;restauro ss
    mov [rsp + 4*8], rax
    mov rax, [savedRegisters + 0x98] ;restauro rsp
    mov [rsp + 3*8], rax
    mov rax, [savedRegisters + 0x90] ;restauro rflags
    mov [rsp + 2*8], rax
    mov rax, [savedRegisters + 0x88] ;restauro cs
    mov [rsp + 8], rax
    mov rax, [savedRegisters + 0x80] ;restauro rip
    mov [rsp], rax
    mov r15, [savedRegisters + 0x78]
    mov r14, [savedRegisters + 0x70]
    mov r13, [savedRegisters + 0x68]
    mov r12, [savedRegisters + 0x60]
    mov r11, [savedRegisters + 0x58]
    mov r10, [savedRegisters + 0x50]
    mov r9, [savedRegisters + 0x48]
    mov r8, [savedRegisters + 0x40]
    mov rbp, [savedRegisters + 0x38]
    mov rdi, [savedRegisters + 0x28]
    mov rsi, [savedRegisters + 0x20]
    mov rdx, [savedRegisters + 0x18]
    mov rcx, [savedRegisters + 0x10]
    mov rbx, [savedRegisters + 0x08]
    mov rax, [savedRegisters + 0x00]
    iretq
    
;Keyboard
_irq01Handler:
    pushState

    ; Leer scancode del teclado
    in al, 0x60
    cmp al, 0x01           ; 0x01 es el scancode de ESC
    jne .skip_save_regs

    mov [savedRegisters + 0x00], rax
    mov [savedRegisters + 0x08], rbx
    mov [savedRegisters + 0x10], rcx
    mov [savedRegisters + 0x18], rdx
    mov [savedRegisters + 0x20], rsi
    mov [savedRegisters + 0x28], rdi
    mov [savedRegisters + 0x30], rsp
    mov [savedRegisters + 0x38], rbp
    mov [savedRegisters + 0x40], r8
    mov [savedRegisters + 0x48], r9
    mov [savedRegisters + 0x50], r10
    mov [savedRegisters + 0x58], r11
    mov [savedRegisters + 0x60], r12
    mov [savedRegisters + 0x68], r13
    mov [savedRegisters + 0x70], r14
    mov [savedRegisters + 0x78], r15
    ;RIP
    mov rax, [rsp + 15*8]  ;direccion de retorno apuntada por rsp
    mov [savedRegisters + 0x80], rax
    mov rax, [rsp + 17*8]
    mov [savedRegisters + 0x88], rax
    mov rax, [savedRegisters]

.skip_save_regs:
    mov rdi, 1 ; pasaje de parametro para irqDispatcher
    call irqDispatcher

    ; fin de la interrupción
    mov al, 20h
    out 20h, al

    popState
    iretq





;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

_irq80Handler:
    pushState
    mov     rdi, rax        ; 1er arg: id de syscall
    mov     rsi, rbx        ; 2º arg
    ; rdx=rdx, rcx=rcx, r8,r9 quedan con 5º y 6º arg

    call    syscallDispatcher

    ; restauro todos los registros menos rax
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
    add rsp, 8  ; Salta pop rax para retornar
    iretq


;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

_exception6Handler:
    exceptionHandler 6

haltcpu:
	cli
	hlt
	ret

section .rodata
userland equ 0x400000

SECTION .bss
	aux resq 1
	global savedRegisters
savedRegisters: resq 21    ; 21 registros de 8 bytes (uint64_t)
temp_rbp: resq 1          ; Variable temporal para guardar RBP original

msj db "TICK",0