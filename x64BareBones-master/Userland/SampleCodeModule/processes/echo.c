#include "../include/userlib.h"
#include "../include/syscall.h"


void echo(uint64_t argc, char **argv) {
    write(satoi(argv[0]), argv[1], strlen(argv[1]));
    my_exit();
}