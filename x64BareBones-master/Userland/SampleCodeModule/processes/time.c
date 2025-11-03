#include "../include/userlib.h"
#include "../include/syscall.h"

void time(uint64_t argc, char **argv){
    printTime();
    my_exit();
}