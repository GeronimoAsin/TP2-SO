#include "../include/userlib.h"
#include "../include/syscall.h"

void ps(uint64_t argc, char **argv) {
    printProcesses();
    my_exit();
} 