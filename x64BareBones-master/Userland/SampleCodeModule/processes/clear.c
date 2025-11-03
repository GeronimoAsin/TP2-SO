#include "../include/userlib.h"
#include "../include/syscall.h"

void clear(uint64_t argc, char **argv){
    clearScreen();
    my_exit();
}