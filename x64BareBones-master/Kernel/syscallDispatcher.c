#include "syscallDispatcher.h"
#include "videoDriver.h"
#include <stdarg.h>

uint64_t syscallDispatcher(uint64_t id, ...)
{
    uint64_t rbx, rdi, rsi, rdx, rcx;
    va_list args;
    va_start(args, id);
    rbx = va_arg(args, uint64_t);
    rdi = va_arg(args, uint64_t);
    rsi = va_arg(args, uint64_t);
    rdx = va_arg(args, uint64_t);
    rcx = va_arg(args, uint64_t);
    va_end(args);

    //switch case para llamar a las syscalls segun id
    switch (id)
    {
        case 0:
            return sys_read(rbx, (char *)rdi, rsi);
        case 1:
            //printChar('1'); // debug int80h.  La syscall de sys_read llega al caso 1
            return sys_write(rbx, (const char *)rdi, rsi);
        case 2:
            //sys_clear
            clearScreen(0x00000000); 
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

    while (i < count) {
        // Espera hasta que haya un caracter disponible
        while ((character = nextFromBuffer()) == 0);
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