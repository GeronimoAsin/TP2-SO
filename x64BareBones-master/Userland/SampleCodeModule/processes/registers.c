#include "../include/userlib.h"
#include "../include/syscall.h"

void registers(uint64_t argc, char **argv){
    print_registers();
    my_exit();
}