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

    mov rdi, %1           ; primer argumento: número de excepción
    mov rsi, [rsp + 15*8] ; segundo argumento: RIP (después de pushState, RIP está en esta posición)
    mov rcx, [rsp + 16*8];  tercer argumento: CS
    mov rdx, [rsp + 17*8];  cuarto argumento: RFLAGS
    call exceptionDispatcher


    popState

    call getStackBase

    mov [rsp], rax

    iretq
%endmacro


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
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
    pushState

    ; Leer scancode del teclado
    in al, 0x60
    cmp al, 0x01           ; 0x01 es el scancode de ESC
    jne .skip_save_regs

    ; Guardar los registros originales en el buffer 'savedRegisters'
    mov rsi, savedRegisters
    mov rax, [rsp + 0*8]   ; rax
    mov [rsi + 0x00], rax
    mov rax, [rsp + 1*8]   ; rbx
    mov [rsi + 0x08], rax
    mov rax, [rsp + 2*8]   ; rcx
    mov [rsi + 0x10], rax
    mov rax, [rsp + 3*8]   ; rdx
    mov [rsi + 0x18], rax
    mov rax, [rsp + 6*8]   ; rsi
    mov [rsi + 0x20], rax
    mov rax, [rsp + 5*8]   ; rdi
    mov [rsi + 0x28], rax
    
    ; RSP original: RSP actual + tamaño registros guardados + tamaño de los valores pusheados por el procesador
    mov rax, rsp
    add rax, 15*8          ; Tamaño de pushState (15 registros)
    add rax, 3*8           ; Tamaño de valores pusheados por CPU (RIP, CS, RFLAGS)
    mov [rsi + 0x30], rax  ; Guardar RSP original
    
    ; Uso el RBP que guardamos antes de modificarlo
    mov rax, [temp_rbp]    ; RBP original guardado en pushState
    mov [rsi + 0x38], rax
    
    mov rax, [rsp + 7*8]   ; r8
    mov [rsi + 0x40], rax
    mov rax, [rsp + 8*8]   ; r9
    mov [rsi + 0x48], rax
    mov rax, [rsp + 9*8]   ; r10
    mov [rsi + 0x50], rax
    mov rax, [rsp + 10*8]  ; r11
    mov [rsi + 0x58], rax
    mov rax, [rsp + 11*8]  ; r12
    mov [rsi + 0x60], rax
    mov rax, [rsp + 12*8]  ; r13
    mov [rsi + 0x68], rax
    mov rax, [rsp + 13*8]  ; r14
    mov [rsi + 0x70], rax
    mov rax, [rsp + 14*8]  ; r15
    mov [rsi + 0x78], rax

    ; RIP está después de todos los registros pusheados por pushState
    mov rax, [rsp + 15*8]  ; RIP (pusheado por CPU antes de entrar al handler)
    mov [rsi + 0x80], rax
    
    ; RFLAGS está después del CS
    mov rax, [rsp + 17*8]  ; RFLAGS
    mov [rsi + 0x88], rax

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

    popState
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
savedRegisters: resq 18    ; 18 registros de 8 bytes (uint64_t)
temp_rbp: resq 1          ; Variable temporal para guardar RBP original