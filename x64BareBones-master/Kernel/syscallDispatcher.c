#include "syscallDispatcher.h"
#include "videoDriver.h"


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