#include "../include/userlib.h"
#include "../include/syscall.h"

extern void print_registers(void);
extern void my_exit(void);

void registers(uint64_t argc, char **argv){
    print_registers();
    my_exit();
}