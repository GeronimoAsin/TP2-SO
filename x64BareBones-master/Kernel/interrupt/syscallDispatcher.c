#include <lib.h>


static uint64_t sys_read(uint64_t fd, char *buff)
{
    if (fd != 0)
    {
        return -1;
    }


    //falta implementar getCharFromKeyboard
    *buff = getCharFromKeyboard();
    return 0;
}

static uint64_t sys_write(uint64_t fd, char buffer)
{
    if (fd != 1)
    {
        return -1;
    }

    //videoDriver imprime el buffer en pantalla  
    return 1;
}
