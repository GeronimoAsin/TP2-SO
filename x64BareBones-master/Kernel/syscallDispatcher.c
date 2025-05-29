#include "syscallDispatcher.h"
#include "videoDriver.h"
uint64_t sys_read(uint64_t fd, char *buff)
{
    if (fd != 0)
    {
        return -1;
    }


    //falta implementar getCharFromKeyboard
   // *buff = getCharFromKeyboard();
    return 0;
}

uint64_t sys_write(uint64_t fd, const char* buffer)
{
    if (fd != 1)
    {
        return -1;
    }

    if(*buffer=='\n')
      {
      	newLine();
      }else
        {
        printString(buffer,0xFFFFFF);
        }

    return 1;
}
