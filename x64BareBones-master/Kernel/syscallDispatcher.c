#include "syscallDispatcher.h"
#include "videoDriver.h"
#include <stdarg.h>
#define REGISTERS 16
//struct para obtener el tiempo
typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} Time;

uint64_t sys_getRegisters(uint64_t *dest);
extern uint64_t getTime();
extern uint64_t getRegisters();

uint64_t syscallDispatcher(uint64_t id, ...)
{
    uint64_t rbx, rdx, rcx, r8, r9;
    va_list args;
    va_start(args, id);
    rbx = va_arg(args, uint64_t);
    rdx = va_arg(args, uint64_t);
    rcx = va_arg(args, uint64_t);
    r8 = va_arg(args, uint64_t);
    r9 = va_arg(args, uint64_t);
    va_end(args);

    //switch case para llamar a las syscalls segun id
    switch (id)
    {
        case 0:
            return sys_read(rbx, (char*) rdx, rcx);
        case 1:
            //printChar('1'); // debug int80h.  La syscall de sys_read llega al caso 1
            return sys_write(rbx, (const char *)rdx, rcx);
        case 2:
            clearScreen(0x00000000); 
            return 1;
        case 3:
            return sys_getTime((Time *) rdx);
        case 4:
            return sys_getRegisters((uint64_t *)rbx);
        case 5:
            deleteLastChar();
            return 1;
		case 6:
			beep();
			return 1;
		case 7:
			increaseFontSize();
			return 1;
		case 8:
			decreaseFontSize();
			return 1;
        case 9:
          	drawCursor();
            return 1;
        case 10:
          	clearCursor();
            return 1;
        case 11:
            drawCircle(rbx, rdx, rcx, (uint32_t)r8);
            return 1;
        case 12: 
            drawRectangle(rbx, rdx, rcx, r8, (uint32_t)r9);
            return 1;
        case 13: 
            clearScreen((uint32_t)rbx);
            return 1;
        case 14: 
            setCursor((int)rbx, (int)rdx);
            return 1;
        case 15: 
            return sys_write(rbx, (const char *)rdx, rcx);
        case 16: 
            return sys_read(rbx, (char*) rdx, rcx);
        case 17: 
            deleteLastChar();
            return 1;
        case 18: 
            hideCursor();
            return 1;
        case 19:
            showCursor();
            return 1;
        case 20:
            *((int *)rbx) = getWidth();
            return 1;
        case 21:
            *((int *)rbx) = getHeight();
            return 1;
        default:
            return -1;
    }   
}


uint64_t sys_read(uint64_t fd, char *buff, uint64_t count)
{
    if (fd != 0)
        return -1;

    uint64_t i = 0;
    char character;

    while (i < count && (character = nextFromBuffer()) != 0) {
        // Espera hasta que haya un caracter disponible
        //while ((character = nextFromBuffer()) == 0);
        buff[i++] = character;
    }
    return i;
}


uint64_t sys_write(uint64_t fd, const char *buffer, uint64_t count)
{
    if (fd != 1)
        return -1;

    uint64_t i;
    for (i = 0; i < count; i++) {
        if (buffer[i] == '\n')
            newLine();
        else
            printChar(buffer[i]);
    }

    return i; // Retorna la cantidad de caracteres escritos

}

void sys_getTime(Time *t)
{
    t->hours = getHours(); //obtengo las horas
    t->minutes = getMinutes(); //obtengo los minutos
    t->seconds = getSeconds(); //obtengo los segundos
}


uint64_t sys_getRegisters(uint64_t *dest) {
    if (dest == 0){
        return -1;
	}
    uint64_t *regs = getRegisters();
    for (int i = 0; i < REGISTERS; i++)
        dest[i] = regs[i]; //copia los registros a la direccion de memoria dest

    return 1;
}
